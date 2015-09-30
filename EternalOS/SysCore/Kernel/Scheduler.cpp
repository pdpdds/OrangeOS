#include "Scheduler.h"
#include "DebugDisplay.h"
#include "mmngr_phys.h"
#include "ProcessManager.h"
#include "Process.h"
#include "Thread.h"

Scheduler* Scheduler::m_pScheduler = 0;
extern int kkk;
void SwitchTask(int tick)
{
	//if (tick == 100)
		Scheduler::GetInstance()->DoSchedule(tick);
}

Scheduler::Scheduler()
{
}


Scheduler::~Scheduler()
{
}
extern void SampleLoop2();
bool  Scheduler::DoSchedule(int tick)
{

	//DebugPrintf("\nScheduler");

	int entryPoint = 0;
	unsigned int procStack = 0;

	/* switch to process address space */
	/*Process* pProcess = ProcessManager::GetInstance()->g_pCurProcess;

	if (pProcess && pProcess->dwRunState == PROCESS_STATE_ACTIVE)
	{
		
		pProcess->dwRunState = PROCESS_STATE_SLEEP;
		
		Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);
		entryPoint = pThread->frame.eip;
		procStack = pThread->frame.esp;
		DebugPrintf("\nScheduler\n");
		
		__asm {
			mov     ax, 0x23; user mode data selector is 0x20 (GDT entry 3).Also sets RPL to 3
				mov     ds, ax
				mov     es, ax
				mov     fs, ax
				mov     gs, ax
				;
			; create stack frame
				;
			push   0x23; SS, notice it uses same selector as above
				push[procStack]; stack
				push    0x200; EFLAGS
				push    0x1b; CS, user mode code selector is 0x18.With RPL 3 this is 0x1b
				push[entryPoint]; EIP
				iretd
		}
	}*/

	if (kkk == 1)
	{
		kkk = 5;
		int entryPoint = (int)SampleLoop2;
		unsigned int procStack = 0;

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
	}
	
	return true;
}
