#include "Process.h"
#include "Thread.h"

Process::Process()
{
	m_taskId = -1;
	dwTickCount = 0;
	m_kernelStackIndex = 0;
	dwWaitingTime = 2;
}


Process::~Process()
{
}

BOOL Process::AddThread(Thread* pThread)
{
	return m_threadList.Add(pThread);
}

Thread* Process::GetThread(int index)
{
	return (Thread*)m_threadList.Get(index);
}

BOOL Process::DelThread(void* ptr)
{
	return m_threadList.Delete(ptr);
}

extern void install_pagedirectory(void* pPageDirectory);

void Process::SetPDBR()
{
	install_pagedirectory(pPageDirectory);
}

Thread* Process::GetRunningThread()
{
	for (int index = 0; index < GetThreadCount(); index++)
	{
		Thread* pThread = GetThread(index);
		if (pThread->state == PROCESS_STATE_RUNNING)
			return pThread;
	}

	return NULL;
}


