#pragma once

#define CODE_SELECTOR 0x8
 #define DATA_SELECTOR 0x10
 
 typedef void(_cdecl *I86_IRQ_HANDLER)(void);

#define ENTER_CRITICAL_SECTION()	__asm	PUSHFD	__asm CLI
#define EXIT_CRITICAL_SECTION()		__asm	POPFD

/*
0x00000000-0x00400000 ?Kernel reserved
0x00400000-0x80000000 ?User land
0x80000000-0xffffffff ?Kernel reserved
*/
#define KE_USER_START   0x00400000
#define KE_KERNEL_START 0x80000000

#define MAX_THREAD 5

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
