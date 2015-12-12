#include "Scheduler.h"
#include "DebugDisplay.h"
#include "mmngr_phys.h"
#include "ProcessManager.h"
#include "Process.h"
#include "Thread.h"
#include "../Hal/Hal.h"
#include "../hal/tss.h"

Scheduler* Scheduler::m_pScheduler = 0;
extern int kkk;
extern tss_entry TSS;
void SwitchTask(int tick, uint32_t registers)
{
	//if (tick == 100)
	Scheduler::GetInstance()->DoSchedule(tick, registers);
}

Scheduler::Scheduler()
{
}


Scheduler::~Scheduler()
{
}
extern void SampleLoop2();
extern int systemOn;
static int aa = 0;
bool  Scheduler::DoSchedule(int tick, uint32_t registers)
{

	//DebugPrintf("\nScheduler");

	int entryPoint = 0;
	unsigned int procStack = 0;
	
	/* switch to process address space */

	if (systemOn == 1)
	{
		Process* curProcess = ProcessManager::GetInstance()->g_pCurProcess;

		if (curProcess)
		{
			Thread* pCurThread = (Thread*)List_GetData(curProcess->pThreadQueue, "", 0);			
			pCurThread->curESP = (*(uint32_t*)(registers - 8));
			pCurThread->curFlags = (*(uint32_t*)(registers - 12));
			pCurThread->curCS = (*(uint32_t*)(registers - 16));
			pCurThread->curEip = (*(uint32_t*)(registers - 20));

			/*pCurThread->curgs = (*(uint16_t*)(registers - 24));
			pCurThread->curfs = (*(uint16_t*)(registers - 28));
			pCurThread->curEs = (*(uint16_t*)(registers - 32));
			pCurThread->curds = (*(uint16_t*)(registers - 36));*/

			pCurThread->frame.eax = (*(uint32_t*)(registers - 24));
			pCurThread->frame.ecx = (*(uint32_t*)(registers - 28));
			pCurThread->frame.edx = (*(uint32_t*)(registers - 32));
			pCurThread->frame.ebx = (*(uint32_t*)(registers - 36));
			pCurThread->frame.esp = (*(uint32_t*)(registers - 40));
			pCurThread->frame.ebp = (*(uint32_t*)(registers - 44));
			pCurThread->frame.esi = (*(uint32_t*)(registers - 48));
			pCurThread->frame.edi = (*(uint32_t*)(registers - 52));			

			LISTNODE *pProcessList = ProcessManager::GetInstance()->pProcessQueue;
			int processCount = List_Count(pProcessList);

			for (int index = 0; index < processCount; index++)
			{
				Process* pProcess = (Process*)List_GetData(pProcessList, "", index);

				if (curProcess == pProcess)
					continue;

						



				pProcess->dwTickCount++;
				if (pProcess->dwTickCount > 10)
				{
					
					pProcess->dwTickCount = 0;
					if (pProcess->dwProcessType == PROCESS_USER)
					{
						

						DebugPrintf("\nScheduler1 : %d", processCount);
						ProcessManager::GetInstance()->g_pCurProcess = pProcess;
						Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);						


						entryPoint = pThread->curEip;
						procStack = pThread->curESP;
						uint32_t eflags = pThread->curFlags;
						


						DebugPrintf("\neip : %x", pThread->curEip);
						DebugPrintf("\nCS : %x", pThread->curCS);
						DebugPrintf("\nflags : %x", pThread->curFlags);
						DebugPrintf("\nesp : %x", pThread->curESP);


						//entryPoint = pThread->frame.eip;						
						//procStack = pThread->frame.esp;						

						uint32_t regEax = pThread->frame.eax;
						uint32_t regEcx = pThread->frame.ecx;
						uint32_t regEdx = pThread->frame.edx;
						uint32_t regEbx = pThread->frame.ebx;
						uint32_t regEsp = pThread->frame.esp;
						uint32_t regEbp = pThread->frame.ebp;
						uint32_t regEsi = pThread->frame.esi;
						uint32_t regEdi = pThread->frame.edi;

						uint16_t regGs = pThread->curgs;
						uint16_t regFs = pThread->curfs;
						uint16_t regEs = pThread->curEs;
						uint16_t regDs = pThread->curds;

						
						pmmngr_load_PDBR((physical_addr)pProcess->pPageDirectory);						
						outportb(0x20, 0x20);
						__asm {
							mov     ax, 0x23; user mode data selector is 0x20 (GDT entry 3).Also sets RPL to 3
								mov     ds, ax
								mov     es, ax
								mov     fs, ax
								mov     gs, ax

							
							; create stack frame
								;
							push   0x23; SS, notice it uses same selector as above
								push[procStack]; stack
								push[eflags]; EFLAGS
								push    0x1b; CS, user mode code selector is 0x18.With RPL 3 this is 0x1b
								push[entryPoint]; EIP

								mov eax, regEax
								mov ecx, regEcx
								mov edx, regEdx
								mov ebx, regEbx
								mov esp, regEsp
								mov ebp, regEbp
								mov esi, regEsi
								mov edi, regEdi

								iretd
						}

					}
					else
					{
						/*if (aa == 0)
						{
							aa = 1;
							DebugPrintf("\nasdasas : %x", registers);
							for (int i = 0; i < 20; i++)
								DebugPrintf("\nasdasas : %x", (*(uint32_t*)(registers - i * 4)));
							
							while (1);
						}*/

						DebugPrintf("\nScheduler2");
						ProcessManager::GetInstance()->g_pCurProcess = pProcess;
						Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);
						entryPoint = pThread->frame.eip;
						procStack = pThread->frame.esp;
						//procStack = 0;

						pmmngr_load_PDBR((physical_addr)pProcess->pPageDirectory);
						outportb(0x20, 0x20);
						__asm {
							mov     ax, 0x10;
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
								push    0x08;
								push[entryPoint]; EIP
								iretd
						}
					}
				}
				
			}
			
		}
	}
	
	/*
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

	/*if (kkk == 1)
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
	}*/
	
	return true;
}
