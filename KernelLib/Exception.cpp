#include "Exception.h"
#include "printf.h"
#include "Console.h"
#include "sprintf.h"

#ifdef _MSC_VER
#define interrupt __declspec (naked)
#else
#define interrupt
#endif

#define far
#define near

extern Console console;

static char* sickpc = " \
                               _______      \n\
                               |.-----.|    \n\
                               ||x . x||    \n\
                               ||_.-._||    \n\
                               `--)-(--`    \n\
                              __[=== o]___  \n\
                             |:::::::::::|\\ \n\
                             `-=========-`()\n\
                                M. O. S.\n\n";

//! something is wrong--bail out
void _cdecl kernel_panic (const char* fmt, ...) {

	_asm cli

	va_list		args;
	static char buf[1024];

	va_start (args, fmt);
	va_end (args);

	char* disclamer="We apologize, MOS has encountered a problem and has been shut down\n\
to prevent damage to your computer. Any unsaved work might be lost.\n\
We are sorry for the inconvenience this might have caused.\n\n\
Please report the following information and restart your computer.\n\
The system has been halted.\n\n";

	console.Clear();	
	//console.WriteString(sickpc, White, Black);
	//console.WriteString(disclamer, White, Black);	
	//sprintf(buf, "*** STOP: %s", fmt);	
	console.WriteString((char *)fmt, White, Black);
	//DebugPuts (sickpc);
	//DebugPuts (disclamer);

	//DebugPrintf ("*** STOP: %s", fmt);

	for (;;);
}


//extern void _cdecl kernel_panic (const char* fmt, ...);

#define intstart() \
	_asm	cli \
	_asm	sub		ebp, 4


#pragma warning (disable:4100)

//! divide by 0 fault
I86_IRQ_HANDLER divide_by_zero_fault (UINT eflags,UINT cs,UINT eip, UINT other) {

	_asm {
		cli
		add esp, 12
		pushad
	}

	kernel_panic ("Divide by 0 at physical address [0x%x:0x%x] EFLAGS [0x%x] other: 0x%x",cs,eip, eflags, other);
	for (;;);
}

//! single step
void interrupt _cdecl single_step_trap (unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Single step");
	for (;;);
}

//! non maskable interrupt trap
void interrupt _cdecl nmi_trap (unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("NMI trap");
	for (;;);
}

//! breakpoint hit
void interrupt _cdecl breakpoint_trap (unsigned int cs,unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Breakpoint trap");
	for (;;);
}

//! overflow
void interrupt _cdecl overflow_trap (unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Overflow trap");
	for (;;);
}

//! bounds check
void interrupt _cdecl bounds_check_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Bounds check fault");
	for (;;);
}

//! invalid opcode / instruction
void interrupt _cdecl invalid_opcode_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Invalid opcode");
	for (;;);
}

//! device not available
void interrupt _cdecl no_device_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Device not found");
	for (;;);
}

//! double fault
void interrupt _cdecl double_fault_abort (unsigned int cs, unsigned int err, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Double fault");
	for (;;);
}

//! invalid Task State Segment (TSS)
void interrupt _cdecl invalid_tss_fault (unsigned int cs,unsigned int err,  unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Invalid TSS");
	for (;;);
}

//! segment not present
void interrupt _cdecl no_segment_fault (unsigned int cs,unsigned int err,  unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Invalid segment");
	for (;;);
}

//! stack fault
void interrupt _cdecl stack_fault ( unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Stack fault");
	for (;;);
}

//! general protection fault
void interrupt _cdecl general_protection_fault (unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("General Protection Fault");
	for (;;);
}


//! page fault
void _cdecl page_fault (UINT err,UINT eflags,UINT cs,UINT eip) {

	_asm	cli
	_asm	sub		ebp, 4

//	int faultAddr=0;

//	_asm {
//		mov eax, cr2
//		mov [faultAddr], eax
//	}

//	kernel_panic ("Page Fault at 0x%x:0x%x refrenced memory at 0x%x",
//		cs, eip, faultAddr);

	_asm popad
	_asm sti
	_asm iretd
}

//! Floating Point Unit (FPU) error
void interrupt _cdecl fpu_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("FPU Fault");
	for (;;);
}

//! alignment check
void interrupt _cdecl alignment_check_fault (unsigned int cs,unsigned int err, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Alignment Check");
	for (;;);
}

//! machine check
void interrupt _cdecl machine_check_abort (unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("Machine Check");
	for (;;);
}

//! Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
void interrupt _cdecl simd_fpu_fault (unsigned int cs, unsigned int eip, unsigned int eflags) {

	intstart ();
	kernel_panic ("FPU SIMD fault");
	for (;;);
}
