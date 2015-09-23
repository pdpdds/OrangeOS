#pragma once
#include "mmngr_virtual.h"
#include "task.h"

class Process;
class Thread;

class ProcessManager
{
public:
	ProcessManager();
	virtual ~ProcessManager();

	Process* CreateMemoryProcess(void(*lpStartAddress)());
	Thread* CreateThread(Process* pProcess);
};

