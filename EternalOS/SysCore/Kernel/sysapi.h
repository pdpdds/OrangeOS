#pragma once
#include <hal.h>
#include "DebugDisplay.h"
#include "mmngr_virtual.h"
#include "task.h"

#define MAX_SYSCALL 6

static void* _syscalls[] = {

	DebugPrintf,
	TerminateProcess,
	MemoryAlloc,
	MemoryFree,
	CreateDefaultHeap,
	GetSysytemTickCount
};

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
int __cdecl _purecall();
void operator delete[](void *p);

bool InitializeMemorySystem(multiboot_info* bootinfo, uint32_t kernelSize);
void InitializeSysCall();

void CreateKernelHeap(int kernelSize);

void sleep(int ms);