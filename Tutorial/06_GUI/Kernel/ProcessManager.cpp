#include "ProcessManager.h"
#include "Process.h"
#include "Hal.h"
#include "Task.h"
#include "Thread.h"
#include "string.h"
#include "Console.h"
#include "Process.h"
#include "task.h"
#include "PhysicalMemoryManager.h"
#include "KernelProcedure.h"

ProcessManager ProcessManager::m_processManager;
extern Console console;

ProcessManager::ProcessManager()
{
	m_nextProcessId = 1;	
}

ProcessManager::~ProcessManager()
{
}

Thread* ProcessManager::CreateThread(Process* pProcess, FILE* file, LPVOID param)
{
	unsigned char buf[512];
	IMAGE_DOS_HEADER* dosHeader = 0;
	IMAGE_NT_HEADERS* ntHeaders = 0;
	unsigned char* memory = 0;

	/* read 512 bytes into buffer */
	volReadFile(file, buf, 512);
	if (!ValidatePEImage(buf)) {
		volCloseFile(file);
		return 0;
	}

	dosHeader = (IMAGE_DOS_HEADER*)buf;
	ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)buf);

	Thread* pThread = new Thread();
	pThread->m_pParent = pProcess;
	pProcess->m_imageBase = ntHeaders->OptionalHeader.ImageBase;
	pProcess->m_imageSize = ntHeaders->OptionalHeader.SizeOfImage;
	pThread->kernelStack = 0;
	pThread->m_dwPriority = 1;
	pThread->m_taskState = TASK_STATE_INIT;
	pThread->m_initialStack = 0;
	pThread->m_stackLimit = PAGE_SIZE;
	pThread->m_imageBase = ntHeaders->OptionalHeader.ImageBase;
	pThread->m_imageSize = ntHeaders->OptionalHeader.SizeOfImage;
	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)ntHeaders->OptionalHeader.AddressOfEntryPoint + ntHeaders->OptionalHeader.ImageBase;
	pThread->frame.flags = 0x200;
	

	int pageRest = 0;

	if ((pThread->m_imageSize % 4096) > 0)
		pageRest = 1;

	pProcess->m_dwPageCount = (pThread->m_imageSize / 4096) + pageRest;
	 
	 memory = (unsigned char*)PhysicalMemoryManager::GetInstance()->AllocBlocks(pProcess->m_dwPageCount);
	
	/* map page into address space */
	for (int i = 0; i <pProcess->m_dwPageCount; i++)
	{		
		VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(pProcess->m_pPageDirectory,
			ntHeaders->OptionalHeader.ImageBase + i * PAGE_SIZE,
			(uint32_t)memory + i * PAGE_SIZE,
			I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);
	}
	memset(memory, 0, pThread->m_imageSize);
	memcpy(memory, buf, 512);

	/* load image into memory */

	int fileRest = 0;
	if ((pThread->m_imageSize % 512) != 0)
		fileRest = 1;

	int readCount = (pThread->m_imageSize / 512) + fileRest;
	for (int i = 1; i < readCount; i++) {
		if (file->eof == 1)
			break;
		volReadFile(file, memory + 512 * i, 512);
	}
		
	/* close file and return process ID */
	volCloseFile(file);

	/* Create userspace stack (process esp=0x100000) */
	void* stack = (void*)(ntHeaders->OptionalHeader.ImageBase + ntHeaders->OptionalHeader.SizeOfImage + PAGE_SIZE);
	void* stackPhys = (void*)PhysicalMemoryManager::GetInstance()->AllocBlock();
	
	/* map user process stack space */
	VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(pProcess->m_pPageDirectory,
		(uint32_t)stack,
		(uint32_t)stackPhys,
		I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);

	/* Create Heap*/
	
	/* final initialization */
	pThread->m_initialStack = stack;
	pThread->frame.esp = (uint32_t)pThread->m_initialStack;
	pThread->frame.ebp = pThread->frame.esp;		

	pThread->frame.eax = 0;
	pThread->frame.ecx = 0;
	pThread->frame.edx = 0;
	pThread->frame.ebx = 0;
	pThread->frame.esi = 0;
	pThread->frame.edi = 0;

	return pThread;

}

Thread* ProcessManager::CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param)
{
	Thread* pThread = new Thread();
	pThread->m_pParent = pProcess;
	
	pThread->m_dwPriority = 1;
	pThread->m_taskState = TASK_STATE_INIT;
	pThread->m_stackLimit = PAGE_SIZE;
	pThread->m_imageBase = 0;
	pThread->m_imageSize = 0;
	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)lpStartAddress;
	pThread->frame.flags = 0x200;
	pThread->m_startParam = param;
	
//������ �����ϰ� �ּҰ����� �����Ѵ�.
	void* stackVirtual = (void*)(KERNEL_VIRTUAL_STACK_ADDRESS + PAGE_SIZE * pProcess->m_kernelStackIndex++);
	void* stackPhys = (void*)PhysicalMemoryManager::GetInstance()->AllocBlock();	

	console.Print("Virtual Stack : %x\n", stackVirtual);
	console.Print("Physical Stack : %x\n", stackPhys);

	/* map user process stack space */
	VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(pProcess->m_pPageDirectory, (uint32_t)stackVirtual, (uint32_t)stackPhys, I86_PTE_PRESENT | I86_PTE_WRITABLE);
	
	pThread->m_initialStack = (void*)((uint32_t)stackVirtual + PAGE_SIZE);
	pThread->frame.esp = (uint32_t)pThread->m_initialStack;
	pThread->frame.ebp = pThread->frame.esp;	

	pProcess->AddThread(pThread);

	ListNode* node = new ListNode();
	node->_data = pThread;
	m_taskList.AddToTail(node);

	return pThread;
}

Process* ProcessManager::CreateProcess(char* appName, UINT32 processType)
{
	FILE file;

	/* open file */
	file = volOpenFile(appName);

	if (file.flags == FS_INVALID)
		return NULL;

	if ((file.flags & FS_DIRECTORY) == FS_DIRECTORY)
		return 0;

	PageDirectory* addressSpace = 0;	
	addressSpace = VirtualMemoryManager::GetInstance()->CreateAddressSpace();

	if (!addressSpace) 
	{		
		volCloseFile(&file);
		return NULL;
	}

	MapKernelSpace(addressSpace);

	Process* pProcess = new Process();

	pProcess->m_processId = ProcessManager::GetInstance()->GetNextProcessId();
	pProcess->m_pPageDirectory = addressSpace;
	pProcess->m_dwPriority = 1;
	pProcess->m_dwRunState = TASK_STATE_INIT;
	strcpy(pProcess->m_processName, appName);

	Thread* pThread = CreateThread(pProcess, &file, NULL);
	pProcess->AddThread(pThread);
	
	return pProcess;
}

Process* ProcessManager::FindProcess(int processId)
{
	for (int i = 0; i < m_processList.Count(); i++)
	{
		Process* pProcess = (Process*)m_processList.Get(i);

		if (pProcess->m_processId == processId)
			return pProcess;
	}

	return NULL;
}

bool ProcessManager::AddProcess(Process* pProcess)
{	
	if (pProcess == 0)
		return false;

	int entryPoint = 0;
	unsigned int procStack = 0;

	if (pProcess->m_processId == PROC_INVALID_ID)
		return false;

	if (!pProcess->m_pPageDirectory)
		return false;

	Thread* pThread = pProcess->GetThread(0);
	
	/* get esp and eip of main thread */
	entryPoint = pThread->frame.eip;
	procStack = pThread->frame.esp;

	console.Print("eip : %x\n", pThread->frame.eip);
	console.Print("page directory : %x\n", pProcess->m_pPageDirectory);
	console.Print("procStack : %x\n", procStack);

	Lock();	
	m_processList.Add(pProcess);
	Unlock();
	
	return true;
}

//firstProcess�� true�� ��� Ŀ���� ���� ���μ����� �����Ѵ�.
//�� ���������� ������ Ŀ������ �����Ǿ��ٰ� �����Ѵ�.
//���� ���μ����� ���⼭ ���� ������ �����Ѵ�.
Process* ProcessManager::CreateProcess(LPTHREAD_START_ROUTINE lpStartAddress, bool firstProcess)
{
	Process* pProcess = new Process();
	pProcess->m_processId = GetNextProcessId();

	if (firstProcess == true)
	{
		pProcess->m_pPageDirectory = VirtualMemoryManager::GetInstance()->GetCurPageDirectory();
		pProcess->m_dwRunState = TASK_STATE_RUNNING;
		strcpy(pProcess->m_processName, "System");
	}
	else
	{
		pProcess->m_pPageDirectory = VirtualMemoryManager::GetInstance()->CreateAddressSpace();
		pProcess->m_dwRunState = TASK_STATE_INIT;
		strcpy(pProcess->m_processName, "System");
		
			PageTable* identityPageTable = (PageTable*)PhysicalMemoryManager::GetInstance()->AllocBlock();
		if (identityPageTable == NULL)
			return false;


		//0-4MB �� ���� �ּҸ� ���� �ּҿ� �����ϰ� ���ν�Ų��
		for (int i = 0, frame = 0x0, virt = 0x00000000; i<PAGES_PER_TABLE; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
		{
			PTE page = 0;
			PageTableEntry::AddAttribute(&page, I86_PTE_PRESENT);
			PageTableEntry::SetFrame(&page, frame);

			identityPageTable->m_entries[PAGE_TABLE_INDEX(virt)] = page;
		}

		

		//���� ���� 1MB ������ ������ �����ּ� ���� 0XC0000000(3GB) ������ ������ ���ν�Ų��
		//Ŀ���� �ε�� ���� ��巹�� �ּ� 0x100000
		// �����ּ� 0XC0000000, 0X100000�� �����ּ� 0x100000�� ���εȴ�
		//���� Ŀ���� ���� PDE, PTE
		for (int y = 0; y < 1; y++)
		{
			PageTable* pTable = (PageTable*)PhysicalMemoryManager::GetInstance()->AllocBlock();
			if (!pTable)
				return false;

#ifdef _ORANGE_DEBUG
			console.Print("Page Table For Kernel Mapping: 0x%x\n", pTable);
#endif // _ORANGE_DEBUG

			//! clear page table
			memset(pTable, 0, sizeof(PageTable));

			int virt = KERNEL_VIRTUAL_BASE_ADDRESS + y * PAGES_PER_TABLE * PAGE_SIZE;

			//Ŀ���� ������� 4�ް��� ���� �ʴ´ٰ� �����Ѵ�
			int frame = KERNEL_PHYSICAL_BASE_ADDRESS;

			for (int i = 0; i < PAGES_PER_TABLE; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
			{
				PTE page = 0;
				PageTableEntry::AddAttribute(&page, I86_PTE_PRESENT);
				PageTableEntry::SetFrame(&page, frame);

				pTable->m_entries[PAGE_TABLE_INDEX(virt)] = page;
			}

			//! get first entry in dir table and set it up to point to our table
			PDE* entry = &pProcess->m_pPageDirectory->m_entries[PAGE_DIRECTORY_INDEX(KERNEL_VIRTUAL_BASE_ADDRESS + y * PAGES_PER_TABLE * PAGE_SIZE)];
			PageDirectoryEntry::AddAttribute(entry, I86_PDE_PRESENT);
			PageDirectoryEntry::AddAttribute(entry, I86_PDE_WRITABLE);
			PageDirectoryEntry::SetFrame(entry, (uint32_t)pTable);
		}

		PDE* identityEntry = &pProcess->m_pPageDirectory->m_entries[PAGE_DIRECTORY_INDEX(0x00000000)];
		PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_PRESENT);
		PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_WRITABLE);
		PageDirectoryEntry::SetFrame(identityEntry, (uint32_t)identityPageTable);

	}

	MapKernelSpace(pProcess->m_pPageDirectory);

	if (firstProcess == false)
	{		
		VirtualMemoryManager::GetInstance()->MapHeap(pProcess->m_pPageDirectory);
	}

	pProcess->m_lpHeap = (void*)(KERNEL_VIRTUAL_HEAP_ADDRESS);

	pProcess->m_dwProcessType = PROCESS_KERNEL;
	pProcess->m_dwPriority = 1;
	
	Thread* pThread = CreateThread(pProcess, lpStartAddress, pProcess);
	
	pProcess->AddThread(pThread);
	AddProcess(pProcess);	

	console.Print("Create Success Task %d\n", pProcess->m_processId);

	return pProcess;
}

bool ProcessManager::MapKernelSpace(PageDirectory* addressSpace)
{
	uint32_t virtualAddr;
	uint32_t physAddr;
	
	int flags = I86_PTE_PRESENT | I86_PTE_WRITABLE;
	
//Ŀ�� �̹����� �ּҰ����� ����. Ŀ�� ũ��� 4�ް��� ���� �ʴ´ٰ� �����Ѵ�
//1024 * PAGE_SIZE = 4MB
	virtualAddr = KERNEL_VIRTUAL_BASE_ADDRESS;
	physAddr = KERNEL_PHYSICAL_BASE_ADDRESS;
	
	for (uint32_t i = 0; i < 1024; i++) 
	{
		VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(addressSpace, virtualAddr + (i*PAGE_SIZE), physAddr + (i*PAGE_SIZE), flags);
	}

	/*
	map display memory for debug minidriver
	idenitity mapped 0xa0000-0xBF000.
	Note:
	A better alternative is to have a driver associated
	with the physical memory range map it. This should be automatic;
	through an IO manager or driver manager.
	*/
	virtualAddr = 0xa0000;
	physAddr = 0xa0000;
	for (uint32_t i = 0; i<31; i++) 
	{
		VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(addressSpace, virtualAddr + (i*PAGE_SIZE), physAddr + (i*PAGE_SIZE), flags);
	}	

	//������ ���丮 ��ü�� �ּҰ����� ����
	VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(addressSpace, (uint32_t)addressSpace, (uint32_t)addressSpace, I86_PTE_PRESENT | I86_PTE_WRITABLE);		

	return true;
}

Process* ProcessManager::GetCurrentProcess()
{
	ListNode* pNode = m_taskList.GetHead();
	if (pNode == NULL)
		return NULL;

	Thread* pThread = (Thread*)pNode->_data;

	return pThread->m_pParent;
}

bool ProcessManager::RemoveFromTaskList(Process* pProcess)
{
	int threadCount = pProcess->m_threadList.CountItems();

	for (int i = 0; i < threadCount; i++)
	{
		Thread* pThread = pProcess->GetThread(i);	

		ListNode* pNode = m_taskList.Remove(pThread);

		if (pNode)
		{
			delete pNode;
		}
		else
		{
			console.Print("task delete fail %d\n", pProcess->m_processId);
		}
	}

	return true;
}

bool ProcessManager::DestroyProcess(Process* pProcess)
{
	//�½�ũ ��Ͽ��� ���μ����� �½�ũ���� �����Ѵ�.
	RemoveFromTaskList(pProcess);

	//������ ���� Context �ڿ��� �����Ѵ�.
	//���� �ּҸ� ��ϱ� ���� �Ҵ��ߴ� ������ ���̺�, ���� ���� ȸ�� ���
	ReleaseThreadContext(pProcess);

	for (uint32_t page = 0; page < pProcess->m_dwPageCount; page++)
	{					
		uint32_t virt = pProcess->m_imageBase + (page * PAGE_SIZE);

		VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(pProcess->m_pPageDirectory, virt);
	}

// �� �޸� ȸ��
	u32int heapAddess = (u32int)pProcess->m_lpHeap;
	heapAddess = heapAddess - (heapAddess % PAGE_SIZE);
	for (int i = 0; i < DEFAULT_HEAP_PAGE_COUNT; i++)
	{
		uint32_t virt = heapAddess + (i * PAGE_SIZE);
		VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(pProcess->m_pPageDirectory, virt);
	}

	//������ ���丮 ȸ��
	//������ ���丮�� ���� �ּ���
	PhysicalMemoryManager::GetInstance()->FreeBlock(pProcess->m_pPageDirectory);	
	m_processList.Delete(pProcess);
	//���μ��� ��ü ������ ����	
#ifdef _ORANGE_DEBUG
	console.Print("terminate %s\n", pProces->m_processName);
#endif // _ORANGE_DEBUG

	delete pProcess;

	return true;
}

//Ŀ�� ���μ����� �����Ѵ�.
bool ProcessManager::DestroyKernelProcess(Process* pProcess)
{
	//�½�ũ ��Ͽ��� ���μ����� �½�ũ���� �����Ѵ�.
	RemoveFromTaskList(pProcess);

	//������ ���� Context �ڿ��� �����Ѵ�.
	//���� �ּҸ� ��ϱ� ���� �Ҵ��ߴ� ������ ���̺�, ���� ���� ȸ�� ���
	ReleaseThreadContext(pProcess);
	
	//������ ���丮 ȸ��
	//������ ���丮�� ���� �ּ���
	PhysicalMemoryManager::GetInstance()->FreeBlock(pProcess->m_pPageDirectory);
	m_processList.Delete(pProcess);
	//���μ��� ��ü ������ ����	
#ifdef _ORANGE_DEBUG
	console.Print("terminate %s\n", pProces->m_processName);
#endif // _ORANGE_DEBUG

	delete pProcess;

	return true;
}

bool ProcessManager::ReleaseThreadContext(Process* pProces)
{
	int threadCount = pProces->m_threadList.CountItems();

	for (int i = 0; i < threadCount; i++)
	{
		Thread* pThread = pProces->GetThread(i);
		//���� ������ ȸ��
		VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(pProces->m_pPageDirectory, (uint32_t)pThread->m_initialStack);

		// TLS ����� ȸ��		
	}

	return true;
}
