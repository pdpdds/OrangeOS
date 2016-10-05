#pragma once
#include "windef.h"
#include "VirtualMemoryManager.h"
#include "TaskStateSegment.h"
#include "kheap.h"
#include "DoubleLinkedList.h"

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

	void SetPDBR();

	UINT32 m_processId;
	UINT32 dwRunState;
	UINT32 dwPriority;
	int dwWaitingTime;
	UINT32 dwPageCount;
	UINT32 dwProcessType;
	UINT32 dwTickCount;
	char processName[256];

	PageDirectory* m_pPageDirectory;	
	void* lpHeap;
		
	int m_kernelStackIndex;
	DoubleLinkedList m_threadList;

	uint32_t  imageBase;
	uint32_t  imageSize;

//Deprecated
	TaskStateSegment* pTSS;

private:
	
};

