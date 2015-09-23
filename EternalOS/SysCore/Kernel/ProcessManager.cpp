#include "ProcessManager.h"
#include "Process.h"
#include "task.h"
#include "Thread.h"

ProcessManager::ProcessManager()
{
}


ProcessManager::~ProcessManager()
{
}

Process* ProcessManager::CreateMemoryProcess(void(*lpStartAddress)())
{
	Process* pProcess = new Process();
	pProcess->TaskID = 1;
	pProcess->pPageDirectory = vmmngr_createAddressSpace();
	mapKernelSpace(pProcess->pPageDirectory);

	pProcess->dwPriority = 1;
	pProcess->dwRunState = PROCESS_STATE_ACTIVE;
	
	Thread* pThread = CreateThread(pProcess);
	List_Add(&pProcess->pThreadQueue, "", pThread);
	

	return 0;
}

Thread* ProcessManager::CreateThread(Process* pProcess)
{
	Thread* pThread = new Thread();
	pThread->

	return pThread;
	
}




;
mainThread->kernelStack = 0;
mainThread->parent = proc;
mainThread->priority = 1;
mainThread->state = PROCESS_STATE_ACTIVE;
mainThread->initialStack = 0;
mainThread->stackLimit = (void*)((uint32_t)mainThread->initialStack + 4096);
mainThread->imageBase = ntHeaders->OptionalHeader.ImageBase;
mainThread->imageSize = ntHeaders->OptionalHeader.SizeOfImage;
memset(&mainThread->frame, 0, sizeof(trapFrame));
mainThread->frame.eip = ntHeaders->OptionalHeader.AddressOfEntryPoint
+ ntHeaders->OptionalHeader.ImageBase;
mainThread->frame.flags = 0x200;


/* Create userspace stack (process esp=0x100000) */
void* stack =
(void*)(ntHeaders->OptionalHeader.ImageBase
+ ntHeaders->OptionalHeader.SizeOfImage + PAGE_SIZE);
void* stackPhys = (void*)pmmngr_alloc_block();

/* map user process stack space */
vmmngr_mapPhysicalAddress(addressSpace,
	(uint32_t)stack,
	(uint32_t)stackPhys,
	I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);

/* final initialization */
mainThread->initialStack = stack;
mainThread->frame.esp = (uint32_t)mainThread->initialStack;
mainThread->frame.ebp = mainThread->frame.esp;
