#include "Scheduler.h"
#include "Console.h"
#include "mmngr_phys.h"
#include "ProcessManager.h"
#include "Process.h"
#include "Thread.h"
#include "Hal.h"
#include "tss.h"

Scheduler* Scheduler::m_pScheduler = 0;
extern int kkk;
extern tss_entry TSS;
extern Console console;


void SwitchTask(int tick, uint32_t registers)
{
	//if (tick == 100)
	Scheduler::GetInstance()->DoSchedule(tick, registers);
}

void SwitchTask(int tick, registers_t regs)
{
	//if (tick == 100)
	Scheduler::GetInstance()->DoScheduleA(tick, regs);
}

Scheduler::Scheduler()
{
}


Scheduler::~Scheduler()
{
}
extern void SampleLoop2();
extern bool systemOn;

bool  Scheduler::DoSchedule(int tick, uint32_t registers)
{
	int entryPoint = 0;
	unsigned int procStack = 0;

	/* switch to process address space */

	if (systemOn == true)
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

						//console.Print("Scheduler1 : %d\n", processCount);
						ProcessManager::GetInstance()->g_pCurProcess = pProcess;
						Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);


						entryPoint = pThread->curEip;
						procStack = pThread->curESP;
						uint32_t eflags = pThread->curFlags;

						//console.Print("eip : %x\n", pThread->curEip);
						//console.Print("CS : %x\n", pThread->curCS);
						//console.Print("flags : %x\n", pThread->curFlags);
						//console.Print("esp : %x\n", pThread->curESP);

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
						OutPortByte(0x20, 0x20);

						if (pThread->dwRunState == PROCESS_STATE_INIT)
						{
							pThread->dwRunState = PROCESS_STATE_RUNNING;
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

									iretd
							}
						}
						else if (pThread->dwRunState == PROCESS_STATE_RUNNING)
						{
							__asm
							{
								mov     ax, 0x23;
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
									mov esp, regEsp
									mov ebp, regEbp
									mov ebx, regEbx
									mov esi, regEsi
									mov edi, regEdi

									iretd
							}
						}


					}
					/*else if (pProcess->dwProcessType == PROCESS_KERNEL)
					{

						ProcessManager::GetInstance()->g_pCurProcess = pProcess;
						Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);


						//entryPoint = pThread->curEip;
						//procStack = pThread->curESP;
						//uint32_t eflags = pThread->curFlags;


						uint32_t regEax = pThread->frame.eax;
						uint32_t regEcx = pThread->frame.ecx;
						uint32_t regEdx = pThread->frame.edx;
						uint32_t regEbx = pThread->frame.ebx;
						uint32_t regEsp = pThread->frame.esp;
						uint32_t regEbp = pThread->frame.ebp;
						uint32_t regEsi = pThread->frame.esi;
						uint32_t regEdi = pThread->frame.edi;
						int entryPoint = (int)pThread->frame.eip;
						unsigned int procStack = pThread->frame.esp;

						pmmngr_load_PDBR((physical_addr)pProcess->pPageDirectory);
						OutPortByte(0x20, 0x20);

						__asm {
							mov     ax, 0x10;
							mov     ds, ax
								mov     es, ax
								mov     fs, ax
								mov     gs, ax
								;
							; create stack frame
								;
							push   0x10;
							push[procStack]; stack
								push    0x200; EFLAGS
								push    0x08;
							push[entryPoint]; EIP


								mov eax, regEax
								mov ecx, regEcx
								mov edx, regEdx
								mov esp, regEsp
								mov ebp, regEbp
								mov ebx, regEbx
								mov esi, regEsi
								mov edi, regEdi

								iretd
						}
					}*/
					else if (pProcess->dwProcessType == PROCESS_KERNEL)
					{

						ProcessManager::GetInstance()->g_pCurProcess = pProcess;
						Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);


						entryPoint = pThread->curEip;
						procStack = pThread->curESP;
						uint32_t eflags = pThread->curFlags;

						uint32_t regEax = pThread->frame.eax;
						uint32_t regEcx = pThread->frame.ecx;
						uint32_t regEdx = pThread->frame.edx;
						uint32_t regEbx = pThread->frame.ebx;
						uint32_t regEsp = pThread->frame.esp;
						uint32_t regEbp = pThread->frame.ebp;
						uint32_t regEsi = pThread->frame.esi;
						uint32_t regEdi = pThread->frame.edi;

						//int entryPoint = (int)pThread->frame.eip;
						unsigned int procStack = pThread->frame.esp;

						pmmngr_load_PDBR((physical_addr)pProcess->pPageDirectory);
						OutPortByte(0x20, 0x20);

						__asm {
							mov     ax, 0x10;
							mov     ds, ax
								mov     es, ax
								mov     fs, ax
								mov     gs, ax
								;
							; create stack frame
								;
							push   0x10;
							push[procStack]; stack
								push[eflags]; EFLAGS
push    0x08;
push[entryPoint]; EIP

mov eax, regEax
mov ecx, regEcx
mov edx, regEdx
mov esp, regEsp
mov ebp, regEbp
mov ebx, regEbx
mov esi, regEsi
mov edi, regEdi

iretd
						}
					}
				}

			}

		}
	}

	/*
	if (pProcess && pProcess->dwRunState == PROCESS_STATE_INIT)
	{

		pProcess->dwRunState = PROCESS_STATE_SLEEP;

		Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);
		entryPoint = pThread->frame.eip;
		procStack = pThread->frame.esp;

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

int static count1 = 0;
bool  Scheduler::DoScheduleA(int tick, registers_t regs)
{
	int entryPoint = 0;
	unsigned int procStack = 0;

	/* switch to process address space */

	if (systemOn == true)
	{
		count1++;
		if ((count1 % 200) == 0)
		{
			//console.Print("Scheduler4 : %d\n", count1);
		}

		Process* curProcess = ProcessManager::GetInstance()->g_pCurProcess;
		
		if (curProcess)
		{
			Thread* pCurThread = (Thread*)List_GetData(curProcess->pThreadQueue, "", 0);
			pCurThread->m_regs = regs;


			pCurThread->curESP = regs.esp;
			pCurThread->curFlags = regs.eflags;
			pCurThread->curCS = regs.cs;
			pCurThread->curEip = regs.eip;

			pCurThread->frame.eax = regs.eax;
			pCurThread->frame.ecx = regs.ecx;
			pCurThread->frame.edx = regs.edx;
			pCurThread->frame.ebx = regs.ebx;
			pCurThread->frame.esp = regs.esp;
			pCurThread->frame.ebp = regs.ebp;
			pCurThread->frame.esi = regs.esi;
			pCurThread->frame.edi = regs.edi;
		}

		LISTNODE *pProcessList = ProcessManager::GetInstance()->pProcessQueue;
		int processCount = List_Count(pProcessList);
		for (int index = 0; index < processCount; index++)
		{
			Process* pProcess = (Process*)List_GetData(pProcessList, "", index);
			Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);

			if (curProcess == pProcess)
				if (curProcess->dwRunState != PROCESS_STATE_SLEEP)
					continue;

			pProcess->dwTickCount++;
			if (pProcess->dwTickCount > 10)
			{
				pProcess->dwTickCount = 0;
				if (pProcess->dwProcessType == PROCESS_USER)
				{
					static int count = 0;
					//console.Print("Scheduler1 : %d\n", count++);
					Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);

					entryPoint = pThread->curEip;
					procStack = pThread->curESP;
					uint32_t eflags = pThread->curFlags;

					/*console.Print("eip : %x\n", pThread->curEip);
					console.Print("CS : %x\n", pThread->curCS);
					console.Print("flags : %x\n", pThread->curFlags);
					console.Print("esp : %x\n", pThread->curESP);
					console.Print("count : %d\n", count++);*/

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

					if (pThread->dwRunState == PROCESS_STATE_INIT)
					{
						ProcessManager::GetInstance()->g_pCurProcess = pProcess;
						pmmngr_load_PDBR((physical_addr)pProcess->pPageDirectory);
						OutPortByte(0x20, 0x20);
						pThread->dwRunState = PROCESS_STATE_RUNNING;
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

								iretd
						}
					}
					else if (pThread->dwRunState == PROCESS_STATE_RUNNING)
					{
						//console.Print("Scheduler3 : %d\n", processCount);
						//console.Print("eip : %x\n", pThread->curEip);
						//console.Print("CS : %x\n", pThread->curCS);
						//console.Print("flags : %x\n", pThread->curFlags);
						//console.Print("esp : %x\n", pThread->curESP);
					//	console.Print("UserESP : %x\n", pThread->m_regs.useresp);
						ProcessManager::GetInstance()->g_pCurProcess = pProcess;
						pmmngr_load_PDBR((physical_addr)pProcess->pPageDirectory);
						OutPortByte(0x20, 0x20);
						pThread->dwRunState = PROCESS_STATE_RUNNING;

						uint32_t regEax = pThread->frame.eax;
						uint32_t regEcx = pThread->frame.ecx;
						uint32_t regEdx = pThread->frame.edx;
						uint32_t regEbx = pThread->frame.ebx;
						uint32_t regEsp = pThread->frame.esp;
						uint32_t regEbp = pThread->frame.ebp;
						uint32_t regEsi = pThread->frame.esi;
						uint32_t regEdi = pThread->frame.edi;


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
								mov esp, regEsp
								mov ebp, regEbp
								mov ebx, regEbx
								mov esi, regEsi
								mov edi, regEdi

								iretd
						}

					}

				}
				else if (pProcess->dwProcessType == PROCESS_KERNEL)
				{
					pProcess->dwRunState = PROCESS_STATE_RUNNING;
					//console.Print("Scheduler2 : %d\n", processCount);
					ProcessManager::GetInstance()->g_pCurProcess = pProcess;
					Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);


					entryPoint = pThread->curEip;
					procStack = pThread->curESP;
					uint32_t eflags = pThread->curFlags;
					uint32_t ssa = pThread->m_regs.ss;
					uint32_t useresp = pThread->m_regs.useresp;

					if (ssa != 0)
						useresp = ssa;

					//entryPoint = pThread->frame.eip;
					//eflags = pThread->frame.flags;

					//console.Print("eip : %x\n", pThread->m_regs.eip);
					//console.Print("CS : %x\n", pThread->m_regs.cs);
					//console.Print("flags : %x\n", pThread->m_regs.eflags);
					//console.Print("useresp : %x\n", pThread->m_regs.useresp);
					//console.Print("ss : %x\n", pThread->m_regs.ss);




					uint32_t regEax = pThread->frame.eax;
					uint32_t regEcx = pThread->frame.ecx;
					uint32_t regEdx = pThread->frame.edx;
					uint32_t regEbx = pThread->frame.ebx;
					uint32_t regEsp = pThread->frame.esp;
					uint32_t regEbp = pThread->frame.ebp;
					uint32_t regEsi = pThread->frame.esi;
					uint32_t regEdi = pThread->frame.edi;


					/*console.Print("regEax : %x\n", regEax);
					console.Print("regEcx : %x\n", regEcx);
					console.Print("regEdx : %x\n", regEdx);
					console.Print("regEbx : %x\n", regEbx);
					console.Print("regEsp : %x\n", regEsp);
					console.Print("regEbp : %x\n", regEbp);
					console.Print("regEsi : %x\n", regEsi);
					console.Print("regEdi : %x\n", regEdi);*/

					pmmngr_load_PDBR((physical_addr)pProcess->pPageDirectory);
					OutPortByte(0x20, 0x20);

					//tss_set_stack(0x10, procStack);

					__asm {

						mov     ax, 0x10;
						mov     ds, ax
							mov     es, ax
							mov     fs, ax
							mov     gs, ax


							push   0x10;
						push[procStack]; stack
							push[eflags]; EFLAGS
							push    0x08;
						push[entryPoint]; EIP

							mov eax, regEax
							mov ecx, regEcx
							mov edx, regEdx
							mov esp, regEsp
							mov ebp, regEbp
							mov ebx, regEbx
							mov esi, regEsi
							mov edi, regEdi

							iretd
					}
				}

			}
		}

	}


	return true;
}
