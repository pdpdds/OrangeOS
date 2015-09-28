#include "Scheduler.h"
#include "DebugDisplay.h"

Scheduler* Scheduler::m_pScheduler = 0;

void SwitchTask(int tick)
{
	if (tick == 100)
		Scheduler::GetInstance()->DoSchedule(tick);
}

Scheduler::Scheduler()
{
}


Scheduler::~Scheduler()
{
}

bool  Scheduler::DoSchedule(int tick)
{

	//DebugPrintf("\nScheduler");
	
	return true;
}
