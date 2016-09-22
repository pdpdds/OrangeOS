#include "Scheduler.h"
#include "Console.h"
#include "PhysicalMemoryManager.h"
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
	return true;
}

int static count1 = 0;
bool  Scheduler::DoScheduleA(int tick, registers_t regs)
{	
	return true;
}
