#include "ProcessManager.h"
#include "Process.h"
#include "Hal.h"
#include "Task.h"
#include "Thread.h"
#include "string.h"
#include "Console.h"
#include "Process.h"
#include "task.h"

ProcessManager* ProcessManager::m_pProcessManager = 0;
extern Console console;

ProcessManager::ProcessManager()
{
	m_nextProcessId = 1;
	m_pCurrentProcess = 0;
}

ProcessManager::~ProcessManager()
{
}

Process* ProcessManager::CreateProcess(LPTHREAD_START_ROUTINE lpStartAddress)
{
	Process* pProcess = new Process();
	pProcess->TaskID = GetNextProcessId();
	pProcess->pPageDirectory = vmmngr_createAddressSpace();
	mapKernelSpace(pProcess->pPageDirectory);

	pProcess->dwPriority = 1;
	pProcess->dwRunState = PROCESS_STATE_INIT;
	
	Thread* pThread = CreateThread(pProcess, lpStartAddress);
	pProcess->AddThread(pThread);
	
	console.Print("Create Success Task %d\n", pProcess->TaskID);

	return pProcess;
}

Thread* ProcessManager::CreateThread(Process* pProcess, FILE* file)
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
	pThread->pParent = pProcess;

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

	memory = (unsigned char*)pmmngr_alloc_blocks(pProcess->dwPageCount);
	
	/* map page into address space */
	for (int i = 0; i <pProcess->dwPageCount; i++)
	{		
		vmmngr_mapPhysicalAddress(pProcess->pPageDirectory,
			ntHeaders->OptionalHeader.ImageBase + i * 4096,
			(uint32_t)memory + i * 4096,
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
	void* stackPhys = (void*)pmmngr_alloc_block();

	/* map user process stack space */
	vmmngr_mapPhysicalAddress(pProcess->pPageDirectory,
		(uint32_t)stack,
		(uint32_t)stackPhys,
		I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);

	/* Create Heap*/
	
	/* final initialization */
	pThread->initialStack = stack;
	pThread->frame.esp = (uint32_t)pThread->initialStack;
	pThread->frame.ebp = pThread->frame.esp;	

	pThread->curEip = pThread->frame.eip;
	pThread->curCS = pThread->curEip;
	pThread->curESP = pThread->frame.esp;
	pThread->curFlags = pThread->frame.flags;	

	pThread->frame.eax = 0;
	pThread->frame.ecx = 0;
	pThread->frame.edx = 0;
	pThread->frame.ebx = 0;
	pThread->frame.esi = 0;
	pThread->frame.edi = 0;

	return pThread;

}

Thread* ProcessManager::CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress)
{
	Thread* pThread = new Thread();
	pThread->pParent = pProcess;

	pThread->kernelStack = 0;
	pThread->priority = 1;
	pThread->state = PROCESS_STATE_INIT;
	pThread->initialStack = 0;
	pThread->stackLimit = (void*)((uint32_t)pThread->initialStack + 4096);
	pThread->imageBase = 0;
	pThread->imageSize = 0;
	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)lpStartAddress;
	pThread->frame.flags = 0x200;

	/* Create userspace stack (process esp=0x100000) */
	void* stack = (void*)0x200000;
	void* stackPhys = (void*)pmmngr_alloc_block();

	/* map user process stack space */
	vmmngr_mapPhysicalAddress(pProcess->pPageDirectory,
		(uint32_t)stack,
		(uint32_t)stackPhys,
		I86_PTE_PRESENT | I86_PTE_WRITABLE);

	/* final initialization */
	pThread->initialStack = stack;
	pThread->frame.esp = (uint32_t)pThread->initialStack;
	pThread->frame.ebp = pThread->frame.esp;

	return pThread;
}

Process* ProcessManager::CreateProcess(char* appname, UINT32 processType)
{
	FILE file;

	/* open file */
	file = volOpenFile(appname);
	if (file.flags == FS_INVALID)
		return 0;
	if ((file.flags & FS_DIRECTORY) == FS_DIRECTORY)
		return 0;

	pdirectory* addressSpace = 0;

	addressSpace = vmmngr_createAddressSpace();
	if (!addressSpace) {		
		volCloseFile(&file);
		return 0;
	}

	mapKernelSpace(addressSpace);

	Process* pProcess = new Process();

	pProcess->TaskID = ProcessManager::GetInstance()->GetNextProcessId();
	pProcess->pPageDirectory = addressSpace;
	pProcess->dwPriority = 1;
	pProcess->dwRunState = PROCESS_STATE_INIT;
	strcpy(pProcess->processName, appname);

	Thread* pThread = CreateThread(pProcess, &file);
	pProcess->AddThread(pThread);	
	
	return pProcess;
}

bool ProcessManager::AddProcess(Process* pProcess)
{	
	if (pProcess == 0)
		return false;

	int entryPoint = 0;
	unsigned int procStack = 0;

	if (pProcess->TaskID == PROC_INVALID_ID)
		return false;

	if (!pProcess->pPageDirectory)
		return false;

	Thread* pThread = pProcess->GetThread(0);
	
	/* get esp and eip of main thread */
	entryPoint = pThread->frame.eip;
	procStack = pThread->frame.esp;

	console.Print("eip : %x\n", pThread->frame.eip);
	console.Print("page directory : %x\n", pProcess->pPageDirectory);	

	Lock();	
	m_processList.Add(pProcess);
	Unlock();
	
	return true;
}

extern void run();
DWORD WINAPI StartRoutine(LPVOID parameter)
{
	while (1) {
		run();
	}	

	for (;;);

	return 0;
}

Process* ProcessManager::CreateSystemProcess()
{
	Process* pProcess = new Process();
	pProcess->TaskID = ProcessManager::GetInstance()->GetNextProcessId();
	pProcess->pPageDirectory = vmmngr_get_directory();
	pProcess->dwProcessType = PROCESS_KERNEL;
	mapKernelSpace(pProcess->pPageDirectory);

	pProcess->dwPriority = 1;
	pProcess->dwRunState = PROCESS_STATE_RUNNING;

	Thread* pThread = CreateThread(pProcess, StartRoutine);
	
	pProcess->AddThread(pThread);
	AddProcess(pProcess);	

	return pProcess;
}