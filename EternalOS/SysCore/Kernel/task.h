
#ifndef _TASK_H
#define _TASK_H
//****************************************************************************
//**
//**    task.h
//**		-Task manager
//**
//****************************************************************************
//============================================================================
//    INTERFACE REQUIRED HEADERS
//============================================================================

#include <stdint.h>
#include "mmngr_virtual.h"
#include "windef.h"
//============================================================================
//    INTERFACE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

/*
0x00000000-0x00400000 ?Kernel reserved
0x00400000-0x80000000 ?User land
0x80000000-0xffffffff ?Kernel reserved
*/
#define KE_USER_START   0x00400000
#define KE_KERNEL_START 0x80000000

#define MAX_THREAD 5

//============================================================================
//    INTERFACE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    INTERFACE STRUCTURES / UTILITY CLASSES
//============================================================================

typedef struct _TSS_32 {
	WORD	prev_task_link, res0;
	DWORD	esp0;
	WORD	ss0, res8;
	DWORD	esp1;
	WORD	ss1, res16;
	DWORD	esp2;
	WORD	ss2, res24;
	DWORD	cr3;
	int		eip;
	DWORD	eflags;
	DWORD	eax;
	DWORD	ecx;
	DWORD	edx;
	DWORD	ebx;
	DWORD	esp;
	DWORD	ebp;
	DWORD	esi;
	DWORD	edi;
	WORD	es, res72;
	WORD	cs, res76;
	WORD	ss, res80;
	WORD	ds, res84;
	WORD	fs, res88;
	WORD	gs, res92;
	WORD	ldt_set_selector, res96;
	WORD	t_flag, io_map_base_address;
} TSS_32, *PTSS_32;
#define KERNEL_CS				0x0008
#define KERNEL_DS				0x0010
#define USER_CS					0x001b
#define USER_DS					0x0023
#define TASK_SW_SEG				0x0028	/* TSS: instruction 'IRETD' transfers excution to this */
#define INIT_TSS_SEG			0x0030	/* TSS: default tss, used by 'LTR' */
#define TMR_TSS_SEG				0x0038	/* TSS: when occurred an interrupt from system timer 0 */
#define SOFT_TS_TSS_SEG			0x0040	/* TSS: task-switching driven by 'int' instruction */
#define SYSCALL_GATE			0x0048

#define KERNEL_SS				KERNEL_DS
#define USER_SS					USER_DS


#define PROCESS_STATE_SLEEP  0
#define PROCESS_STATE_ACTIVE 1

typedef struct _trapFrame {
   uint32_t esp;
   uint32_t ebp;
   uint32_t eip;
   uint32_t edi;
   uint32_t esi;
   uint32_t eax;
   uint32_t ebx;
   uint32_t ecx;
   uint32_t edx;
   uint32_t flags;
}trapFrame;

struct _process;
typedef struct _thread {
   struct _process*  parent;
   void*     initialStack; /* virtual address */
   void*     stackLimit;
   void*     kernelStack;
   uint32_t  priority;
   int       state;
   trapFrame frame;
   uint32_t  imageBase;
   uint32_t  imageSize;
}thread;

typedef struct _process {
   int            id;
   int            priority;
   pdirectory*    pageDirectory;
   int            state;
	/* threadCount will always be 1 */
   int threadCount;
   struct _thread  threads[MAX_THREAD];
}process;

//============================================================================
//    INTERFACE DATA DECLARATIONS
//============================================================================
//============================================================================
//    INTERFACE FUNCTION PROTOTYPES
//============================================================================

extern int createThread    (int (*entry) (void), uint32_t stackBase);
extern int terminateThread (thread* handle);

class Process;

extern "C" void TerminateProcess ();

extern "C" uint32_t MemoryAlloc(size_t size);
extern "C" void MemoryFree(void* p);

extern "C" void CreateDefaultHeap();

extern "C" void TerminateMemoryProcess();

extern "C" uint32_t GetSysytemTickCount();


void mapKernelSpace(pdirectory* addressSpace);
int validateImage(void* image);

//============================================================================
//    INTERFACE OBJECT CLASS DEFINITIONS
//============================================================================
//============================================================================
//    INTERFACE TRAILING HEADERS
//============================================================================
//****************************************************************************
//**
//**    END [task.h]
//**
//****************************************************************************

#endif