#pragma once
#include "VirtualMemoryManager.h"
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

		
	Process* GetCurrentProcess() { return m_pCurrentProcess; }
	void SetCurrentProcess(Process* pProcess) { m_pCurrentProcess = pProcess; }


	Orange::LinkedList* GetProcessList() { return &m_processList;}
	int GetNextProcessId() { return m_nextProcessId++; }

	static ProcessManager* GetInstance()
	{
		if (m_pProcessManager == 0)
			m_pProcessManager = new ProcessManager();

		return m_pProcessManager;
	}
	
	Process* CreateProcess(LPTHREAD_START_ROUTINE lpStartAddress, bool firstProcess = false);
	Process* CreateProcess(char* appName, UINT32 processType);	
	Thread* CreateThread(Process* pProcess, FILE* pFile);
	Thread* CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress);

	bool AddProcess(Process* pProces);	

//Page Directory Mapping
	bool MapKernelSpace(PageDirectory* addressSpace);	

private:	

private:
	static ProcessManager* m_pProcessManager;

	int m_nextProcessId;
	Process* m_pCurrentProcess;
	Orange::LinkedList m_processList;	
};