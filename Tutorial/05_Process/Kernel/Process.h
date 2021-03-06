#pragma once
#include "windef.h"
#include "mmngr_virtual.h"
#include "TaskStateSegment.h"
#include "kheap.h"
#include "List.h"

#define PROCESS_USER 0
#define PROCESS_KERNEL 1

class Process
{
public:
	Process();
	virtual ~Process();

	void SetPDBR();

	UINT32 TaskID;
	UINT32 dwRunState;
	UINT32 dwPriority;
	UINT32 dwWaitingTime;
	UINT32 dwPageCount;
	UINT32 dwProcessType;
	UINT32 dwTickCount;

	pdirectory* pPageDirectory;
	TaskStateSegment* pTSS;
	heap_t* pHeap;
	LPLISTNODE pThreadQueue;


};

