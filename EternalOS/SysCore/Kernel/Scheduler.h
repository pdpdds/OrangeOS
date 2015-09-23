#pragma once
#include "kheap.h"

class Scheduler
{
public:
	static Scheduler* GetInstance()
	{
		if (m_pScheduler == 0)
			m_pScheduler = new Scheduler();

		return m_pScheduler;
	}

	bool  DoSchedule(int tick);

private:
	Scheduler();
	~Scheduler();

	static Scheduler* m_pScheduler;
};

