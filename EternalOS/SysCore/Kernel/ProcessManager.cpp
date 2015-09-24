#include "ProcessManager.h"
#include "Process.h"
#include "task.h"
#include "Thread.h"
#include "DebugDisplay.h"
#include "string.h"

ProcessManager* ProcessManager::m_pProcessManager = 0;

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

	//pProcess->pPageDirectory = vmmngr_get_directory();

	pProcess->dwPriority = 1;
	pProcess->dwRunState = PROCESS_STATE_ACTIVE;
	
	Thread* pThread = CreateMemoryThread(pProcess, lpStartAddress);
	List_Add(&pProcess->pThreadQueue, "", pThread);
	
	List_Add(&pProcessQueue, "", pProcess);

	DebugPrintf("\nCreate Success Memory Task");
	
	int entryPoint = 0;
	unsigned int procStack = 0;

	/* get esp and eip of main thread */
	entryPoint = pThread->frame.eip;
	procStack = pThread->frame.esp;

	/* switch to process address space */
	__asm cli
	pmmngr_load_PDBR((physical_addr)pProcess->pPageDirectory);

	/* execute process in user mode */
	__asm {
		mov     ax, 0x10; user mode data selector is 0x20 (GDT entry 3).Also sets RPL to 3
			mov     ds, ax
			mov     es, ax
			mov     fs, ax
			mov     gs, ax
			;
		; create stack frame
			;
		push   0x10; SS, notice it uses same selector as above
			push[procStack]; stack
			push    0x200; EFLAGS
			push    0x08; CS, user mode code selector is 0x18.With RPL 3 this is 0x1b
			push[entryPoint]; EIP
			iretd
	}

	return 0;
}

Thread* ProcessManager::CreateMemoryThread(Process* pProcess, void(*lpStartAddress)())
{
	Thread* pThread = new Thread();
	pThread->pParent = pProcess;

	pThread->kernelStack = 0;
	pThread->priority = 1;
	pThread->state = PROCESS_STATE_ACTIVE;
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