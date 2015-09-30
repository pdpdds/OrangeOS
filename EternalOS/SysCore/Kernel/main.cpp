/******************************************************************************
   main.cpp
		-Kernel main program

   modified\ Oct 10 2010
   arthor\ Mike
******************************************************************************/

/**
Chapter 23
----------

KNOWN BUGS:

-There is a bug found in get_cmd(). It seems to add 1 extra character to the buffer
 when the entered text is all caps. Works fine if its in lowercase

-In volOpenFile(), fsys.cpp: if input contains ':', an additional character, 0x2, is added
 before ':' on input of function. For example, a:main.cpp becomes:

 'a' '' ':' 'm' 'a' 'i' 'n' '.' 'c' 'p' 'p' ''

-. and .. in pathnames also dont seem to work. Need to look into

Currently a workaround is provided in the routine.
Several chapters may need to be updated, please be patient. :-)
**/

#include <bootinfo.h>
#include <hal.h>
#include <kybrd.h>
#include <string.h>
#include <flpydsk.h>
#include <fat12.h>
#include <stdio.h>

#include "DebugDisplay.h"
#include "exception.h"
#include "mmngr_phys.h"
#include "mmngr_virtual.h"
#include "task.h"
#include "kheap.h"
#include "HardDisk.h"
#include "Thread.h"
#include "Process.h"
#include "ProcessManager.h"
/**
*	Memory region
*/
struct memory_region {

	uint32_t	startLo;	//base address
	uint32_t	startHi;
	uint32_t	sizeLo;		//length (in bytes)
	uint32_t	sizeHi;
	uint32_t	type;
	uint32_t	acpi_3_0;
};

uint32_t kernelSize=0;

extern void enter_usermode ();
extern void install_tss (uint32_t idx, uint16_t kernelSS, uint16_t kernelESP);
extern void syscall_init ();

//! set access bit
#define I86_GDT_DESC_ACCESS			0x0001			//00000001

//! descriptor is readable and writable. default: read only
#define I86_GDT_DESC_READWRITE			0x0002			//00000010

//! set expansion direction bit
#define I86_GDT_DESC_EXPANSION			0x0004			//00000100

//! executable code segment. Default: data segment
#define I86_GDT_DESC_EXEC_CODE			0x0008			//00001000

//! set code or data descriptor. defult: system defined descriptor
#define I86_GDT_DESC_CODEDATA			0x0010			//00010000

//! set dpl bits
#define I86_GDT_DESC_DPL			0x0060			//01100000

//! set "in memory" bit
#define I86_GDT_DESC_MEMORY			0x0080			//10000000

/**	gdt descriptor grandularity bit flags	***/

//! masks out limitHi (High 4 bits of limit)
#define I86_GDT_GRAND_LIMITHI_MASK		0x0f			//00001111

//! set os defined bit
#define I86_GDT_GRAND_OS			0x10			//00010000

//! set if 32bit. default: 16 bit
#define I86_GDT_GRAND_32BIT			0x40			//01000000

//! 4k grandularity. default: none
#define I86_GDT_GRAND_4K			0x80			//10000000

void CreateKernelHeap(int kernelSize);


extern VOID HalChangeTssBusyBit(WORD TssSeg, BOOL SetBit);

BOOL HalSetupTaskSwitchingEnv(TSS_32 *pTss32)
{
	int stack;

	ENTER_CRITICAL_SECTION();
	stack = pTss32->esp;
	stack -= sizeof(int);
	*((int *)stack) = pTss32->eflags;
	stack -= sizeof(int);
	*((int *)stack) = pTss32->cs;
	stack -= sizeof(int);
	*((int *)stack) = pTss32->eip;
	pTss32->esp = stack;
	EXIT_CRITICAL_SECTION();

	HalChangeTssBusyBit(TASK_SW_SEG, TRUE);

	return TRUE;
}

bool HalSetupTSS(TSS_32	*pTss32, bool IsKernelTSS, int	EntryPoint, int	*pStackBase, DWORD StackSize)
{
	DWORD dwEFLAGS = 0;
	int stack = (int)pStackBase + StackSize - 1;

	memset(pTss32, NULL, sizeof(TSS_32));

	_asm {
		push		eax

			pushfd
			pop			eax
			or			ah, 02h; IF
			mov			dwEFLAGS, eax

			pop			eax

	}
	
	
	if (IsKernelTSS) {
		pTss32->cs = 0x08;
		pTss32->ds = 0x10;
		pTss32->ss = 0x10;
	}
	else {
		pTss32->cs = USER_CS;
		pTss32->ds = USER_DS;
		pTss32->ss = USER_SS;
	}
	pTss32->es = pTss32->ds;
	pTss32->fs = pTss32->ds;
	pTss32->gs = pTss32->ds;

	pTss32->eflags = dwEFLAGS;
	pTss32->eip = EntryPoint;
	pTss32->esp = (DWORD)pStackBase;

	pTss32->ss0 = KERNEL_SS;
	pTss32->esp0 = ((DWORD)(0x9000));

	HalSetupTaskSwitchingEnv(pTss32);

	return TRUE;
}
#define SYSTEM_TMR_INT_NUMBER	0x20
#define SOFT_TASK_SW_INT_NUMBER	0x30
extern int i86_install_ir(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER irq);
#include "../hal/idt.h"
/**
*	Initialization
*/
void init (multiboot_info* bootinfo) {

	//! initialize our vmm
//	vmmngr_initialize ();

	//! clear and init display
	DebugClrScr (0x13);
	DebugGotoXY (0,0);
	DebugSetColor (0x17);

	hal_initialize ();

	enable ();
	setvect (0,(void (__cdecl &)(void))divide_by_zero_fault);
	setvect (1,(void (__cdecl &)(void))single_step_trap);
	setvect (2,(void (__cdecl &)(void))nmi_trap);
	setvect (3,(void (__cdecl &)(void))breakpoint_trap);
	setvect (4,(void (__cdecl &)(void))overflow_trap);
	setvect (5,(void (__cdecl &)(void))bounds_check_fault);
	setvect (6,(void (__cdecl &)(void))invalid_opcode_fault);
	setvect (7,(void (__cdecl &)(void))no_device_fault);
	setvect (8,(void (__cdecl &)(void))double_fault_abort);
	setvect (10,(void (__cdecl &)(void))invalid_tss_fault);
	setvect (11,(void (__cdecl &)(void))no_segment_fault);
	setvect (12,(void (__cdecl &)(void))stack_fault);
	setvect (13,(void (__cdecl &)(void))general_protection_fault);
	setvect (14,(void (__cdecl &)(void))page_fault);
	setvect (16,(void (__cdecl &)(void))fpu_fault);
	setvect (17,(void (__cdecl &)(void))alignment_check_fault);
	setvect (18,(void (__cdecl &)(void))machine_check_abort);
	setvect (19,(void (__cdecl &)(void))simd_fpu_fault);


	//i86_install_ir(SYSTEM_TMR_INT_NUMBER, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | 0x0500, 0x8, (I86_IRQ_HANDLER)TMR_TSS_SEG);
	i86_install_ir(SOFT_TASK_SW_INT_NUMBER, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | 0x0500, SOFT_TS_TSS_SEG, (I86_IRQ_HANDLER)SOFT_TS_TSS_SEG);

	pmmngr_init ((size_t) bootinfo->m_memorySize, 0xC0000000 + kernelSize*512);
	
	memory_region*	region = (memory_region*)0x1000;

	for (int i=0; i<10; ++i) {

		if (region[i].type>4)
			break;

		if (i>0 && region[i].startLo==0)
			break;

		pmmngr_init_region (region[i].startLo, region[i].sizeLo);
	}
	pmmngr_deinit_region (0x100000, kernelSize*512 + 4096*1024);
	/*
		kernel stack location
	*/
	pmmngr_deinit_region (0x0, 0x10000);

	//! initialize our vmm
	vmmngr_initialize ();

	//! install the keyboard to IR 33, uses IRQ 1
	kkybrd_install (33);

	//! set drive 0 as current drive
	flpydsk_set_working_drive (0);

	//! install floppy disk to IR 38, uses IRQ 6
	flpydsk_install (38);

	//! initialize FAT12 filesystem
	fsysFatInitialize ();

	//! initialize system calls
	syscall_init ();

	//! initialize TSS
	//install_tss (5,0x10,0x9000);
	
	CreateKernelHeap(kernelSize);	

	//for (int i = 1; i < 9; i++)
		//pmmngr_alloc_blocks(3);

	pmmngr_alloc_blocks(3);
	pmmngr_alloc_blocks(3);
	pmmngr_alloc_blocks(3);
	
}

void* pHeapPhys = 0;

void CreateKernelHeap(int kernelSize)
{
	void* pHeap =
		(void*)(0xC0000000 + 409600 + 1024 * 4096);
	
	pHeapPhys = (void*)pmmngr_alloc_blocks(1000);

	for (int i = 0; i < 1000; i++)
	{
		vmmngr_mapPhysicalAddress(vmmngr_get_directory(),
			(uint32_t)pHeap + i * 4096,
			(uint32_t)pHeapPhys + i * 4096,
			I86_PTE_PRESENT | I86_PTE_WRITABLE);
	}

	create_kernel_heap((u32int)pHeap, (uint32_t)pHeap + 1000 * 4096, (uint32_t)pHeap + 1000 * 4096, 0, 0);
}

//! sleeps a little bit. This uses the HALs get_tick_count() which in turn uses the PIT
void sleep (int ms) {

	static int ticks = ms + get_tick_count ();
	while (ticks > get_tick_count ())
		;
}

//! wait for key stroke
KEYCODE	getch () {

	KEYCODE key = KEY_UNKNOWN;

	//! wait for a keypress
	while (key==KEY_UNKNOWN)
		key = kkybrd_get_last_key ();

	//! discard last keypress (we handled it) and return
	kkybrd_discard_last_key ();
	return key;
}

//! command prompt
void cmd () {

	DebugPrintf ("\nCommand> ");
}

//! gets next command
void get_cmd (char* buf, int n) {

	KEYCODE key = KEY_UNKNOWN;
	bool	BufChar;

	//! get command string
	int i=0;
	while ( i < n ) {

		//! buffer the next char
		BufChar = true;

		//! grab next char
		key = getch ();

		//! end of command if enter is pressed
		if (key==KEY_RETURN)
			break;

		//! backspace
		if (key==KEY_BACKSPACE) {

			//! dont buffer this char
			BufChar = false;

			if (i > 0) {

				//! go back one char
				unsigned y, x;
				DebugGetXY (&x, &y);
				if (x>0)
					DebugGotoXY (--x, y);
				else {
					//! x is already 0, so go back one line
					y--;
					x = DebugGetHorz ();
				}

				//! erase the character from display
				DebugPutc (' ');
				DebugGotoXY (x, y);

				//! go back one char in cmd buf
				i--;
			}
		}

		//! only add the char if it is to be buffered
		if (BufChar) {

			//! convert key to an ascii char and put it in buffer
			char c = kkybrd_key_to_ascii (key);
			if (c != 0) { //insure its an ascii char

				DebugPutc (c);
				buf [i++] = c;
			}
		}

		//! wait for next key. You may need to adjust this to suite your needs
		sleep (10);
	}

	//! null terminate the string
	buf [i] = '\0';
}

void *operator new(size_t size)
{
	return (void *)kmalloc(size);
}

void *operator new[](size_t size)
{
	return (void *)kmalloc(size);
}

void operator delete(void *p)
{
	kfree(p);
}

int __cdecl _purecall()
{
	// Do nothing or print an error message.
	return 0;
}

void operator delete[](void *p)
{
	kfree(p);
}

#include "ZetPlane.h"
void cmd_alloc()
{
	for (int i = 0;  i < 1000; i++)
	{
		ZetPlane* pPlane = new ZetPlane();
		pPlane->SetX(i);
		pPlane->SetY(i + 5);

		pPlane->IsRotate();

		DebugPrintf("\n Plane X : %d, Plane Y : %d", pPlane->GetX(), pPlane->GetY());

		delete pPlane;
	}	
}

#include "ProcessManager.h"
#include "List.h"

void SampleLoop2();
int kkk = 0;
void SampleLoop()
{
	char* str = "\n\rHello world2!";

	int first = GetSysytemTickCount();
	while (1)
	{
		static int count = 0;
		int second = GetSysytemTickCount();
		if (second - first > 100)
		{
			DebugPrintf("%s", str);

			first = GetSysytemTickCount();
			count++;
		}
		
		if (count > 5)
		{
			kkk = 1;
			/*int entryPoint = (int)SampleLoop2;
			unsigned int procStack = 0;

			__asm {
				mov     ax, 0x10; user mode data selector is 0x20 (GDT entry 3).Also sets RPL to 3
					mov     ds, ax
					mov     es, ax
					mov     fs, ax
					mov     gs, ax
					;
				; create stack frame
					;
				push   0x10; SS, notice it uses same selector as above
					push[procStack]; stack
					push    0x200; EFLAGS
					push    0x08; CS, user mode code selector is 0x18.With RPL 3 this is 0x1b
					push[entryPoint]; EIP
					iretd
			}*/
		}

	}

	//Process* pProcess = (Process*)List_GetData(manager.pProcessQueue, "", 0);

	//TerminateMemoryProcess();

	for (;;);
}

void SampleLoop2()
{
	char* str = "\n\rHello world3!";

	int first = GetSysytemTickCount();
	while (1)
	{

		int second = GetSysytemTickCount();
		if (second - first > 100)
		{
			DebugPrintf("%s", str);

			first = GetSysytemTickCount();
		}
		

	}

	for (;;);
}

void cmd_memtask()
{
	

	Process* pProcess = ProcessManager::GetInstance()->CreateMemoryProcess(SampleLoop);

	if (pProcess)
	{

		int entryPoint = 0;
		unsigned int procStack = 0;
		Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);

		/* get esp and eip of main thread */
		entryPoint = pThread->frame.eip;
		procStack = pThread->frame.esp;

		/* switch to process address space */
		__asm cli
		pmmngr_load_PDBR((physical_addr)pProcess->pPageDirectory);

		/* execute process in user mode */
		__asm {
			mov     ax, 0x10; user mode data selector is 0x20 (GDT entry 3).Also sets RPL to 3
				mov     ds, ax
				mov     es, ax
				mov     fs, ax
				mov     gs, ax
				;
			; create stack frame
				;
			push   0x10; SS, notice it uses same selector as above
				push[procStack]; stack
				push    0x200; EFLAGS
				push    0x08; CS, user mode code selector is 0x18.With RPL 3 this is 0x1b
				push[entryPoint]; EIP
				iretd
		}
	}

	ProcessManager::GetInstance()->CreateMemoryProcess(SampleLoop2);
	
}

void cmd_memstate() {
	DebugPrintf("\nfree block count %d", pmmngr_get_free_block_count());
	DebugPrintf("\ntotal block count %d", pmmngr_get_block_count());
	DebugPrintf("\n");
	DebugPrintf("\n");
	DebugPrintf("\n");
	DebugPrintf("\n");
}


//! read command
void cmd_read () {

	//! get pathname
	char path[32];
	DebugPrintf ("\n\rex: \"file.txt\", \"a:\\file.txt\", \"a:\\folder\\file.txt\"\n\rFilename> ");
	get_cmd (path,32);

	//! open file
	FILE file = volOpenFile (path);

	//! test for invalid file
	if (file.flags == FS_INVALID) {

		DebugPrintf ("\n\rUnable to open file");
		return;
	}

	//! cant display directories
	if (( file.flags & FS_DIRECTORY ) == FS_DIRECTORY) {

		DebugPrintf ("\n\rUnable to display contents of directory.");
		return;
	}

	//! top line
	DebugPrintf ("\n\n\r-------[%s]-------\n\r", file.name);

	//! display file contents
	while (file.eof != 1) {

		//! read cluster
		unsigned char buf[512];
		volReadFile ( &file, buf, 512);

		//! display file
		for (int i=0; i<512; i++)
			DebugPutc (buf[i]);

		//! wait for input to continue if not EOF
		if (file.eof != 1) {
			DebugPrintf ("\n\r------[Press a key to continue]------");
			getch ();
			DebugPrintf ("\r"); //clear last line
		}
	}

	//! done :)
	DebugPrintf ("\n\n\r--------[EOF]--------");
}

void go_user () {

	int stack=0;
	_asm mov [stack], esp

	extern void tss_set_stack (uint16_t, uint16_t);
	tss_set_stack (0x10,(uint16_t) stack & 0xffff);

	enter_usermode();

	char* testStr ="\n\rWe are inside of your computer...";

	//! call OS-print message
	_asm xor eax, eax
	_asm lea ebx, [testStr]
	_asm int 0x80

	//! cant do CLI+HLT here, so loop instead
	while(1);
}

void run();

// proc (process) command
void cmd_proc () {

	int ret = 0;
	char name[32];

	DebugPrintf ("\n\rProgram file: ");
	get_cmd (name,32);

	Process* pProcess = ProcessManager::GetInstance()->CreateProcess(name);
	if (pProcess == 0)
	{
		DebugPrintf("\n\rError creating process");
		run();
	}
	else		
		ProcessManager::GetInstance()->ExecuteProcess(pProcess);
}

//! our simple command parser
bool run_cmd (char* cmd_buf) {

	if (strcmp (cmd_buf, "user") == 0) {
		go_user ();
	}

	//! exit command
	if (strcmp (cmd_buf, "exit") == 0) {
		return true;
	}

	//! clear screen
	else if (strcmp (cmd_buf, "cls") == 0) {
		DebugClrScr (0x17);
	}

	//! help
	else if (strcmp (cmd_buf, "help") == 0) {

		DebugPuts ("\nOS Development Series Process Management Demo");
		DebugPuts ("Supported commands:\n");
		DebugPuts (" - exit: quits and halts the system\n");
		DebugPuts (" - cls: clears the display\n");
		DebugPuts (" - help: displays this message\n");
		DebugPuts (" - read: reads a file\n");
		DebugPuts (" - reset: Resets and recalibrates floppy for reading\n");
		DebugPuts (" - proc: Run process");
	}

	//! read sector
	else if (strcmp (cmd_buf, "read") == 0) {
		cmd_read ();
	}
	else if (strcmp(cmd_buf, "memstate") == 0) {
		cmd_memstate();
	}
	else if (strcmp(cmd_buf, "alloc") == 0) {
		cmd_alloc();
	}
	else if (strcmp(cmd_buf, "memtask") == 0) {
		cmd_memtask();
	}

	//! run process
	else if (strcmp (cmd_buf, "proc") == 0) {
		cmd_proc();
	}

	//! invalid command
	else {
		DebugPrintf ("\nUnkown command");
	}

	return false;
}

void run () {

	//! command buffer
	char	cmd_buf [100];

	while (1) {

		//! display prompt
		cmd ();

		//! get command
		get_cmd (cmd_buf, 98);

		//! run command
		if (run_cmd (cmd_buf) == true)
			break;
	}
}

int _cdecl kmain (multiboot_info* bootinfo) {

	_asm	mov	word ptr [kernelSize], dx

	init (bootinfo);

	DebugGotoXY (0,0);
	DebugPuts ("OSDev Series Process Management Demo");
	DebugPuts ("\nType \"exit\" to quit, \"help\" for a list of commands\n");
	DebugPuts ("+-------------------------------------------------------+\n");

	/*HardDiskHandler hardHandler;
	hardHandler.Initialize();
	char num = hardHandler.GetTotalDevices();

	DebugPrintf("\nHardDisk Count %d", (int)num);
	*/

	ProcessManager::GetInstance()->CreateSystemProcess();

	_asm {
		push	eax

		pushfd
		pop		eax
		or		ah, 40h ; nested
		push	eax
		popfd

		pop		eax
		iretd
	}


	run ();

	DebugPrintf ("\nExit command recieved; demo halted");
	_asm mov eax, 0xa0b0c0d0
	for (;;);
	return 0;
}
