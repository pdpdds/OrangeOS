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

ProcessManager* ProcessManager::m_pProcessManager = 0;
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
	if (!validateImage(buf)) {
		volCloseFile(file);
		return 0;
	}

	dosHeader = (IMAGE_DOS_HEADER*)buf;
	ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)buf);

	Thread* pThread = new Thread();
	pThread->m_pParent = pProcess;
	pProcess->imageBase = ntHeaders->OptionalHeader.ImageBase;
	pProcess->imageSize = ntHeaders->OptionalHeader.SizeOfImage;
	pThread->kernelStack = 0;
	pThread->priority = 1;
	pThread->state = PROCESS_STATE_INIT;
	pThread->initialStack = 0;
	pThread->stackLimit = (void*)((uint32_t)pThread->initialStack + 4096);
	pThread->imageBase = ntHeaders->OptionalHeader.ImageBase;
	pThread->imageSize = ntHeaders->OptionalHeader.SizeOfImage;
	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)ntHeaders->OptionalHeader.AddressOfEntryPoint + ntHeaders->OptionalHeader.ImageBase;
	pThread->frame.flags = 0x200;
	

	int pageRest = 0;

	if ((pThread->imageSize % 4096) > 0)
		pageRest = 1;

	pProcess->dwPageCount = (pThread->imageSize / 4096) + pageRest;
	 
	 memory = (unsigned char*)PhysicalMemoryManager::GetInstance()->AllocBlocks(pProcess->dwPageCount);
	
	/* map page into address space */
	for (int i = 0; i <pProcess->dwPageCount; i++)
	{		
		VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(pProcess->m_pPageDirectory,
			ntHeaders->OptionalHeader.ImageBase + i * PAGE_SIZE,
			(uint32_t)memory + i * PAGE_SIZE,
			I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);
	}
	memset(memory, 0, pThread->imageSize);
	memcpy(memory, buf, 512);

	/* load image into memory */

	int fileRest = 0;
	if ((pThread->imageSize % 512) != 0)
		fileRest = 1;

	int readCount = (pThread->imageSize / 512) + fileRest;
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
	pThread->initialStack = stack;
	pThread->frame.esp = (uint32_t)pThread->initialStack;
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
	
	pThread->priority = 1;
	pThread->state = PROCESS_STATE_INIT;	
	pThread->stackLimit = (void*)(PAGE_SIZE);
	pThread->imageBase = 0;
	pThread->imageSize = 0;
	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)lpStartAddress;
	pThread->frame.flags = 0x200;
	pThread->startParam = param;
	
//스택을 생성하고 주소공간에 매핑한다.
	void* stackVirtual = (void*)(KERNEL_VIRTUAL_STACK_ADDRESS + PAGE_SIZE * pProcess->m_kernelStackIndex);
	void* stackPhys = (void*)PhysicalMemoryManager::GetInstance()->AllocBlock();
	pProcess->m_kernelStackIndex++;

	console.Print("Virtual Stack : %x\n", stackVirtual);
	console.Print("Physical Stack : %x\n", stackPhys);

	/* map user process stack space */
	VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(pProcess->m_pPageDirectory, (uint32_t)stackVirtual, (uint32_t)stackPhys, I86_PTE_PRESENT | I86_PTE_WRITABLE);
	
	pThread->initialStack = (void*)((uint32_t)stackVirtual + PAGE_SIZE);
	pThread->frame.esp = (uint32_t)pThread->initialStack;
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
	pProcess->dwPriority = 1;
	pProcess->dwRunState = PROCESS_STATE_INIT;
	strcpy(pProcess->processName, appName);

	Thread* pThread = CreateThread(pProcess, &file, NULL);
	pProcess->AddThread(pThread);
	
	return pProcess;
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

//firstProcess가 true일 경우 커널의 최초 프로세스를 생성한다.
//이 시점에서만 이전에 커널힙이 생성되었다고 가정한다.
//이후 프로세스는 여기서 힙을 별도로 생성한다.
Process* ProcessManager::CreateProcess(LPTHREAD_START_ROUTINE lpStartAddress, bool firstProcess)
{
	InterruptDisable();
	Process* pProcess = new Process();
	pProcess->m_processId = GetNextProcessId();

	if (firstProcess == true)
	{
		pProcess->m_pPageDirectory = VirtualMemoryManager::GetInstance()->GetCurPageDirectory();
		pProcess->dwRunState = PROCESS_STATE_RUNNING;
		MapKernelSpace(pProcess->m_pPageDirectory);
	}
	else
	{
		pProcess->m_pPageDirectory = VirtualMemoryManager::GetInstance()->CreateAddressSpace();
		pProcess->dwRunState = PROCESS_STATE_INIT;

		//! allocates 3gb page table
		PageTable* identityPageTable = (PageTable*)PhysicalMemoryManager::GetInstance()->AllocBlock();
		if (identityPageTable == NULL)
			return false;

#ifdef _ORANGE_DEBUG
		console.Print("Identity Page Table Alloc : 0x%x\n", identityPageTable);
#endif // _ORANGE_DEBUG


		//0-4MB 의 물리 주소를 가상 주소와 동일하게 매핑시킨다
		for (int i = 0, frame = 0x0, virt = 0x00000000; i<PAGES_PER_TABLE; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
		{
			PTE page = 0;
			PageTableEntry::AddAttribute(&page, I86_PTE_PRESENT);
			PageTableEntry::SetFrame(&page, frame);

			identityPageTable->m_entries[PAGE_TABLE_INDEX(virt)] = page;
		}
		
		memset(pProcess->m_pPageDirectory, 0, sizeof(PageDirectory));

		//물리 공간 1MB 이후의 공간을 가상주소 공간 0XC0000000(3GB) 이후의 공간과 매핑시킨다
		//커널이 로드된 물리 어드레스 주소 0x100000
		// 가상주소 0XC0000000, 0X100000는 물리주소 0x100000에 매핑된다
		//최초 커널을 위한 PDE, PTE
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

			//커널의 사이즈는 4메가가 넘지 않는다고 가정한다
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

		MapKernelSpace(pProcess->m_pPageDirectory);
	}	
	
	

	pProcess->dwProcessType = PROCESS_KERNEL;
	pProcess->dwPriority = 1;
	
	Thread* pThread = CreateThread(pProcess, lpStartAddress, pProcess);
	
	pProcess->AddThread(pThread);
	AddProcess(pProcess);	

	console.Print("Create Success Task %d\n", pProcess->m_processId);

	InterruptEnable();

	return pProcess;
}

bool ProcessManager::MapKernelSpace(PageDirectory* addressSpace)
{
	uint32_t virtualAddr;
	uint32_t physAddr;
	
	int flags = I86_PTE_PRESENT | I86_PTE_WRITABLE;
	
//커널 이미지를 주소공간에 매핑. 커널 크기는 4메가가 넘지 않는다고 가정한다
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

	//페이지 디렉토리 자체를 주소공간에 매핑
	VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(addressSpace, (uint32_t)addressSpace, (uint32_t)addressSpace, I86_PTE_PRESENT | I86_PTE_WRITABLE);	

	//커널 힙을 메모리에 매핑
	VirtualMemoryManager::GetInstance()->MapHeapSpace(addressSpace);

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

bool ProcessManager::RemoveFromTaskList(Process* pProces)
{
	int threadCount = pProces->m_threadList.CountItems();

	for (int i = 0; i < threadCount; i++)
	{
		Thread* pThread = pProces->GetThread(i);
		ListNode* pNode = m_taskList.Remove(pThread);

		if (pNode)
		{
			delete pNode;
		}
		else
		{
			console.Print("task delete fail %d\n", pProces->m_processId);
		}
	}

	return true;
}

bool ProcessManager::DestroyProcess(Process* pProces)
{
	//태스크 목록에서 프로세스의 태스크들을 제거한다.
	RemoveFromTaskList(pProces);

	//스레드 관련 Context 자원을 해제한다.
	//가상 주소를 운영하기 위해 할당했던 페이지 테이블을 회수 등등
	ReleaseThreadContext(pProces);

	for (uint32_t page = 0; page < pProces->dwPageCount; page++) 
	{					
		uint32_t virt = pProces->imageBase + (page * PAGE_SIZE);

		VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(pProces->m_pPageDirectory, virt);
	}

// 힙 메모리 회수
	u32int heapAddess = (u32int)pProces->lpHeap;
	heapAddess = heapAddess - (heapAddess % PAGE_SIZE);
	for (int i = 0; i < 300; i++)
	{
		uint32_t virt = heapAddess + (i * PAGE_SIZE);
		VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(pProces->m_pPageDirectory, virt);
	}

	//페이지 디렉토리 회수
	PhysicalMemoryManager::GetInstance()->FreeBlock(pProces->m_pPageDirectory);
	
	//프로세스 객체 완전히 제거
	m_processList.Delete(pProces);
	console.Print("terminate %s\n", pProces->processName);
	delete pProces;

	return true;
}

bool ProcessManager::ReleaseThreadContext(Process* pProces)
{
	int threadCount = pProces->m_threadList.CountItems();

	for (int i = 0; i < threadCount; i++)
	{
		Thread* pThread = pProces->GetThread(i);
		//스택 페이지 회수
		VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(pProces->m_pPageDirectory, (uint32_t)pThread->initialStack);

		// TLS 등등을 회수		
	}

	return true;
}
