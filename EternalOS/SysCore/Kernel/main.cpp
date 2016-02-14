
#include "header.h"
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
#include "HardDisk.h"
#include "Thread.h"
#include "Process.h"
#include "ProcessManager.h"
#include "idt.h"
#include "sysapi.h"

extern void enter_usermode ();
extern void install_tss (uint32_t idx, uint16_t kernelSS, uint16_t kernelESP);
extern int i86_install_ir(uint32_t i, uint16_t flags, uint16_t sel, I86_IRQ_HANDLER irq);

void ScreenInit()
{
	DebugClrScr(0x13);
	DebugGotoXY(0, 0);
	DebugSetColor(0x17);
}

void SetInterruptVector()
{
	enable();
	setvect(0, (void(__cdecl &)(void))divide_by_zero_fault);
	setvect(1, (void(__cdecl &)(void))single_step_trap);
	setvect(2, (void(__cdecl &)(void))nmi_trap);
	setvect(3, (void(__cdecl &)(void))breakpoint_trap);
	setvect(4, (void(__cdecl &)(void))overflow_trap);
	setvect(5, (void(__cdecl &)(void))bounds_check_fault);
	setvect(6, (void(__cdecl &)(void))invalid_opcode_fault);
	setvect(7, (void(__cdecl &)(void))no_device_fault);
	setvect(8, (void(__cdecl &)(void))double_fault_abort);
	setvect(10, (void(__cdecl &)(void))invalid_tss_fault);
	setvect(11, (void(__cdecl &)(void))no_segment_fault);
	setvect(12, (void(__cdecl &)(void))stack_fault);
	setvect(13, (void(__cdecl &)(void))general_protection_fault);
	setvect(14, (void(__cdecl &)(void))page_fault);
	setvect(16, (void(__cdecl &)(void))fpu_fault);
	setvect(17, (void(__cdecl &)(void))alignment_check_fault);
	setvect(18, (void(__cdecl &)(void))machine_check_abort);
	setvect(19, (void(__cdecl &)(void))simd_fpu_fault);

	i86_install_ir(SYSTEM_TMR_INT_NUMBER, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | 0x0500, 0x8, (I86_IRQ_HANDLER)TMR_TSS_SEG);
	//i86_install_ir(SOFT_TASK_SW_INT_NUMBER, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | 0x0500, SOFT_TS_TSS_SEG, (I86_IRQ_HANDLER)SOFT_TS_TSS_SEG);
}

void InitializeFloppyDrive()
{
	//! set drive 0 as current drive
	flpydsk_set_working_drive(0);

	//! install floppy disk to IR 38, uses IRQ 6
	flpydsk_install(38);

	//! initialize FAT12 filesystem
	fsysFatInitialize();
}

/**
*	Initialization
*/
void init (multiboot_info* bootinfo, uint32_t kernelSize) 
{
	ScreenInit();	
	hal_initialize();
	SetInterruptVector();
	
	InitializeMemorySystem(bootinfo, kernelSize);

	//! install the keyboard to IR 33, uses IRQ 1
	kkybrd_install (33);

	InitializeFloppyDrive();

	HardDiskHandler hardHandler;
	hardHandler.Initialize();
	char num = hardHandler.GetTotalDevices();

	DebugPrintf("\nHardDisk Count %d", (int)num);

	//! initialize system calls
	InitializeSysCall();

	//! initialize TSS
	install_tss (5,0x10,0x9000);	
}

int systemOn = 0;
int _cdecl kmain(multiboot_info* bootinfo) {

	uint32_t kernelSize = 0;
	_asm	mov	word ptr[kernelSize], dx

	init(bootinfo, kernelSize);

	/*DebugGotoXY(0, 0);
	DebugPuts("OrangeOS Console");
	DebugPuts("\nType \"exit\" to quit, \"help\" for a list of commands\n");
	DebugPuts("+-------------------------------------------------------+\n");	*/

	Process* pProcess = ProcessManager::GetInstance()->CreateSystemProcess();
	ProcessManager::GetInstance()->g_pCurProcess = pProcess;
	systemOn = 1;

	Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);
	int entryPoint = (int)pThread->frame.eip;
	unsigned int procStack = pThread->frame.esp;

	__asm {
		mov     ax, 0x10;
		mov     ds, ax
			mov     es, ax
			mov     fs, ax
			mov     gs, ax
			;
		; create stack frame
			;
		push   0x10;
		push[procStack]; stack
			push    0x200; EFLAGS
			push    0x08;
		push[entryPoint]; EIP
			iretd
	}

	DebugPrintf("\nExit command recieved; demo halted");
	_asm mov eax, 0xa0b0c0d0
	for (;;);
	return 0;
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

	Process* pProcess = ProcessManager::GetInstance()->CreateProcess(name, PROCESS_USER);
	if (pProcess == 0)
	{
		DebugPrintf("\n\rError creating process");		
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
