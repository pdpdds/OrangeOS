#pragma once


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


struct pdirectory;

typedef struct _process {
   int            id;
   int            priority;
   pdirectory*    pageDirectory;
   int            state;
	/* threadCount will always be 1 */
   int threadCount;
   struct _thread  threads[MAX_THREAD];
}process;