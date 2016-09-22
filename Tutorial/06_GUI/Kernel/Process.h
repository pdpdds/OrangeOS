#pragma once
#include "windef.h"
#include "VirtualMemoryManager.h"
#include "TaskStateSegment.h"
#include "kheap.h"
#include "List.h"

#define PROCESS_USER 0
#define PROCESS_KERNEL 1

class Thread;

class Process
{
public:
	Process();
	virtual ~Process();

	BOOL AddThread(Thread* pThread);
	Thread* GetThread(int index);
	BOOL DelThread(void* ptr);

	void SetPDBR();

	UINT32 m_taskId;
	UINT32 dwRunState;
	UINT32 dwPriority;
	UINT32 dwWaitingTime;
	UINT32 dwPageCount;
	UINT32 dwProcessType;
	UINT32 dwTickCount;
	char processName[256];

	PageDirectory* pPageDirectory;
	TaskStateSegment* pTSS;
	heap_t* pHeap;

private:
	Orange::LinkedList m_threadList;

};

