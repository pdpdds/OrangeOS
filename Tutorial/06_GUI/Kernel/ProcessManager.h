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
		return &m_processManager;
	}
	
	Process* CreateProcess(LPTHREAD_START_ROUTINE lpStartAddress, bool firstProcess = false);
	Process* CreateProcess(char* appName, UINT32 processType);	
	Thread* CreateThread(Process* pProcess, FILE* pFile, LPVOID param);
	Thread* CreateThread(Process* pProcess, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);

	bool AddProcess(Process* pProces);
	bool RemoveFromTaskList(Process* pProces);

	bool DestroyProcess(Process* pProces);		
	bool ReleaseThreadContext(Process* pProces);

	DoubleLinkedList* GetTaskList() { return &m_taskList; }

//Page Directory Mapping
	bool MapKernelSpace(PageDirectory* addressSpace);

private:	

private:
	static ProcessManager m_processManager;

	int m_nextProcessId;	
	Orange::LinkedList m_processList;
	DoubleLinkedList m_taskList;
};