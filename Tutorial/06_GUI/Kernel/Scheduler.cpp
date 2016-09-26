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


void SwitchTask(int tick, registers_t registers)
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

bool  Scheduler::DoSchedule(int tick, registers_t registers)
{
	if (systemOn == false)
		return false;

#ifdef _ORANGE_DEBUG
	uint32_t currentTickCount = GetSysytemTickCount();

	if (currentTickCount - lastTickCount > 300)
	{
		lastTickCount = currentTickCount;

		console.Print("\nSwitch Stack Report\n");		
						
		console.Print("DS : %x\n", registers.ds);

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
		console.Print("SS : %x\n", registers.ss);		
	}

#endif
	return true;
}
