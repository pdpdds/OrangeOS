#pragma once
#include "mmngr_virtual.h"
#include "task.h"
#include "List.h"

class Process;
class Thread;

class ProcessManager
{
public:
	ProcessManager();
	virtual ~ProcessManager();

	Process* CreateMemoryProcess(void(*lpStartAddress)());
	Thread* CreateMemoryThread(Process* pProcess, void(*lpStartAddress)());

	LPLISTNODE pProcessQueue;

	static ProcessManager* GetInstance()
	{
		if (m_pProcessManager == 0)
			m_pProcessManager = new ProcessManager();

		return m_pProcessManager;
	}

	static ProcessManager* m_pProcessManager;
};

