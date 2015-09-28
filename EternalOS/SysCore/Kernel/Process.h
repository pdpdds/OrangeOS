#pragma once
#include "windef.h"
#include "mmngr_virtual.h"
#include "TaskStateSegment.h"
#include "kheap.h"
#include "List.h"


class Process
{
public:
	Process();
	virtual ~Process();

	UINT32 TaskID;
	UINT32 dwRunState;
	UINT32 dwPriority;
	UINT32 dwWaitingTime;
	UINT32 dwPageCount;

	pdirectory* pPageDirectory;
	TaskStateSegment* pTSS;
	heap_t* pHeap;
	LPLISTNODE pThreadQueue;


};

