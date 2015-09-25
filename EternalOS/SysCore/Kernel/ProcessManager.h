#pragma once
#include "mmngr_virtual.h"
#include "task.h"
#include "List.h"
#include "image.h"
#include "fsys.h"

class Process;
class Thread;

class ProcessManager
{
public:
	ProcessManager();
	virtual ~ProcessManager();

	int GetNextProcessId(){ return m_nextProcessId++; }

	Process* CreateMemoryProcess(void(*lpStartAddress)());

	Thread* CreateThread(Process* pProcess, FILE* pFile);
	Thread* CreateMemoryThread(Process* pProcess, void(*lpStartAddress)());

	LPLISTNODE pProcessQueue;
	int m_nextProcessId;

	static ProcessManager* GetInstance()
	{
		if (m_pProcessManager == 0)
			m_pProcessManager = new ProcessManager();

		return m_pProcessManager;
	}

	static ProcessManager* m_pProcessManager;
};

