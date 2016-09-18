#include "Process.h"

Process::Process()
{
	TaskID = -1;
	dwTickCount = 0;
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


