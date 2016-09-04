#pragma once
#include <hal.h>
#include "DebugDisplay.h"
#include "mmngr_virtual.h"
#include "task.h"

#define MAX_SYSCALL 7
int printf(const char* str, ...);
HANDLE CreateThread(SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreateionFlags, LPDWORD lpThreadId);

static void* _syscalls[] = {

	printf,
	TerminateProcess,
	MemoryAlloc,
	MemoryFree,
	CreateDefaultHeap,
	GetSysytemTickCount,
	CreateThread,
};

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
void operator delete(void *p, size_t size);
int __cdecl _purecall();
void operator delete[](void *p);


void InitializeSysCall();