#pragma once
#include "mmngr_virtual.h"
#include "task.h"
#include "List.h"
#include "image.h"
#include "fsys.h"
#include "Process.h"
#include "Thread.h"

class Process;
class Thread;

#define PROC_INVALID_ID -1

class ProcessManager
{
public:
	ProcessManager();
	virtual ~ProcessManager();

	int GetNextProcessId(){ return m_nextProcessId++; }
	Process* CreateSystemProcess();
	Process* CreateProcess(char* appname, UINT32 processType);		

	Process* CreateMemoryProcess(void(*lpStartAddress)());	

	Thread* CreateThread(Process* pProcess, FILE* pFile);
	Thread* CreateMemoryThread(Process* pProcess, void(*lpStartAddress)());	

	bool ExecuteProcess(Process* pProces);

	LPLISTNODE pProcessQueue;
	int m_nextProcessId;

	Process* g_pCurProcess;

	static ProcessManager* GetInstance()
	{
		if (m_pProcessManager == 0)
			m_pProcessManager = new ProcessManager();

		return m_pProcessManager;
	}

	static ProcessManager* m_pProcessManager;
};