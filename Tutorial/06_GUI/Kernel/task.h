#pragma once
#include "header.h"
#include "VirtualMemoryManager.h"


extern int createThread    (int (*entry) (void), uint32_t stackBase);
extern int terminateThread (thread* handle);

class Process;

extern "C" void TerminateProcess ();

extern "C" uint32_t MemoryAlloc(size_t size);
extern "C" void MemoryFree(void* p);

extern "C" void CreateDefaultHeap();

extern "C" void TerminateMemoryProcess();

extern "C" uint32_t GetSysytemTickCount();


void mapKernelSpace(PageDirectory* addressSpace);
int validateImage(void* image);