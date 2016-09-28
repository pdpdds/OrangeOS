#include "Scheduler.h"
#include "Console.h"
#include "PhysicalMemoryManager.h"
#include "ProcessManager.h"
#include "Process.h"
#include "Thread.h"
#include "Hal.h"
#include "tss.h"

Scheduler* Scheduler::m_pScheduler = 0;
extern tss_entry TSS;
extern Console console;


void SwitchTask(int tick, registers_t& registers)
{
	Scheduler::GetInstance()->DoSchedule(tick, registers);
}

Scheduler::Scheduler()
{
}


Scheduler::~Scheduler()
{
}

extern bool systemOn;
uint32_t lastTickCount = 0;
extern int g_esp;

bool  Scheduler::DoSchedule(int tick, registers_t& registers)
{
	if (systemOn == false)
		return false;

#ifdef _ORANGE_DEBUG
	uint32_t currentTickCount = GetSysytemTickCount();

	//if (currentTickCount - lastTickCount > 300)
	{
		lastTickCount = currentTickCount;

		/*console.Print("\nSwitch Stack Report\n");

		console.Print("EDI : %x\n", registers.edi);
		console.Print("ESI : %x\n", registers.esi);
		console.Print("EBP : %x\n", registers.ebp);
		console.Print("ESP : %x\n", registers.esp);
		console.Print("EBX : %x\n", registers.ebx);
		console.Print("EDX : %x\n", registers.edx);
		console.Print("ECX : %x\n", registers.ecx);
		console.Print("EAX : %x\n", registers.eax);

		console.Print("EIP : %x\n", registers.eip);
		console.Print("CS : %x\n", registers.cs);
		console.Print("EFLAGS : %x\n", registers.eflags);*/
		//console.Print("USERESP : %x\n", registers.useresp);
		//console.Print("SS : %x\n", registers.ss);			
		
		
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();

		if (pProcess)
		{
			Thread* pThread = pProcess->GetRunningThread();

			if (pThread)
			{
				pThread->dwWaitingTime--;

				if (pThread->dwWaitingTime > 0)
				{
					
					return true;
				}
				else if (pProcess->GetThreadCount() == 1)
				{
					
					pThread->dwWaitingTime = 2;
					return true;
				}
				else
				{


					for (int index = 0; index < pProcess->GetThreadCount(); index++)
					{
						Thread* pNextThread = pProcess->GetThread(index);

						if (pThread == pNextThread)
							continue;

						pThread->m_regs = registers;
						pThread->curESP = g_esp;

						pThread->state = PROCESS_STATE_WAIT;

						if (pNextThread->state == PROCESS_STATE_INIT)
						{
							pNextThread->dwWaitingTime = 2;
							pNextThread->state = PROCESS_STATE_RUNNING;

							int entryPoint = (int)pNextThread->frame.eip;
							unsigned int procStack = pNextThread->frame.esp;

							__asm
							{
								mov     ax, 0x10;
								mov     ds, ax
									mov     es, ax
									mov     fs, ax
									mov     gs, ax
									;
								; create stack frame
									;

								mov esp, procStack
									push    0x200; EFLAGS
									push    0x08; CS
									push entryPoint; EIP

									//sti
									mov al, 0x20
									out 0x20, al

									iretd
							}
						}
						else
						{

							/*console.Print("EDI : %x\n", pNextThread->m_regs.edi);
							console.Print("ESI : %x\n", pNextThread->m_regs.esi);
							console.Print("EBP : %x\n", pNextThread->m_regs.ebp);
							console.Print("ESP : %x\n", pNextThread->m_regs.esp);
							console.Print("EBX : %x\n", pNextThread->m_regs.ebx);
							console.Print("EDX : %x\n", pNextThread->m_regs.edx);
							console.Print("ECX : %x\n", pNextThread->m_regs.ecx);
							console.Print("EAX : %x\n", pNextThread->m_regs.eax);

							console.Print("EIP : %x\n", pNextThread->m_regs.eip);
							console.Print("CS : %x\n", pNextThread->m_regs.cs);
							console.Print("EFLAGS : %x\n", pNextThread->m_regs.eflags);

							console.Print("ds : %x\n", pNextThread->m_regs.ds);
							console.Print("gs : %x\n", pNextThread->m_regs.gs);
							console.Print("es : %x\n", pNextThread->m_regs.es);
							console.Print("fs : %x\n", pNextThread->m_regs.fs);
							*/
							pNextThread->dwWaitingTime = 2;
							pNextThread->state = PROCESS_STATE_RUNNING;
							
							g_esp = pNextThread->curESP;
						}


						break;
					}
				}

			}
		}
	}

#endif
	return true;
}
