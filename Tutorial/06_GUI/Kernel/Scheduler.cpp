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
	lastTickCount = currentTickCount;

	if (currentTickCount - lastTickCount > 300)
	{
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
		console.Print("EFLAGS : %x\n", registers.eflags);
		console.Print("USERESP : %x\n", registers.useresp);
		console.Print("SS : %x\n", registers.ss);*/
	}
#endif

	DoubleLinkedList* pTaskList = ProcessManager::GetInstance()->GetTaskList();

	int taskCount = pTaskList->CountItems();

	if (taskCount == 0)
		return true;

	ListNode* pNode = pTaskList->GetHead();
	Thread* pThread = (Thread*)pNode->_data;

	if (taskCount == 1)
		return true;

	pThread->m_waitingTime--;

	if (pThread->m_waitingTime > 0)
		return true;

	pThread->state = PROCESS_STATE_WAIT;
	pThread->m_regs = registers;
	pThread->curESP = g_esp;
	pTaskList->Remove(pNode);
	pTaskList->AddToTail(pNode);

	ListNode* pCandidate = pTaskList->GetHead();
	Thread* pNextThread = (Thread*)pCandidate->_data;

	if (pNextThread->state == PROCESS_STATE_INIT)
	{
		pNextThread->m_waitingTime = 2;
		pNextThread->state = PROCESS_STATE_RUNNING;

		int entryPoint = (int)pNextThread->frame.eip;
		unsigned int procStack = pNextThread->frame.esp;
		LPVOID param = pNextThread->startParam;

		__asm
		{
			mov     ax, 0x10;
			mov     ds, ax
			mov     es, ax
			mov     fs, ax
			mov     gs, ax
			
			mov esp, procStack

			push   param;
			push    0x10;
			push    0x200; EFLAGS
			push    0x08; CS
			push entryPoint; EIP

			sti
			mov al, 0x20
			out 0x20, al

			iretd
		}
	}
	else
	{


#ifdef _ORANGE_DEBUG
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
		console.Print("fs : %x\n", pNextThread->m_regs.fs);*/
#endif		
		pNextThread->m_waitingTime = 2;
		pNextThread->state = PROCESS_STATE_RUNNING;

		g_esp = pNextThread->curESP;
	}


	return true;
}
