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

		
	Process* GetCurrentProcess();	


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
	Thread* CreateThread(Process* pProcess, FILE* pFile, LPVOID param);
	Thread* CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);

	bool AddProcess(Process* pProces);
	bool RemoveFromTaskList(Process* pProces);

	bool DestroyProcess(Process* pProces);	
	bool ReleaseHeap(Process* pProces);
	bool ReleaseThreadContext(Process* pProces);

	DoubleLinkedList* GetTaskList() { return &m_taskList; }

//Page Directory Mapping
	bool MapKernelSpace(PageDirectory* addressSpace);	

private:	

private:
	static ProcessManager* m_pProcessManager;

	int m_nextProcessId;	
	Orange::LinkedList m_processList;
	DoubleLinkedList m_taskList;
};