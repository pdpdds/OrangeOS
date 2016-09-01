#include "ConsoleManager.h"
#include "string.h"
#include "Console.h"
#include "mmngr_phys.h"
#include "mmngr_virtual.h"
#include "task.h"
#include "DebugDisplay.h"
#include "ProcessManager.h"
#include "ZetPlane.h"

extern Console console;
extern void enter_usermode();

ConsoleManager::ConsoleManager()
{
}


ConsoleManager::~ConsoleManager()
{
}

void cmd_alloc()
{
	for (int i = 0; i < 1000; i++)
	{
		ZetPlane* pPlane = new ZetPlane();
		pPlane->SetX(i);
		pPlane->SetY(i + 5);

		pPlane->IsRotate();

		DebugPrintf("\n Plane X : %d, Plane Y : %d", pPlane->GetX(), pPlane->GetY());

		delete pPlane;
	}
}

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

/* video mode info. */
#define WIDTH           800
#define HEIGHT          600
#define BPP             32
#define BYTES_PER_PIXEL 4

/* BGA LFB is at LFB_PHYSICAL. */
#define LFB_PHYSICAL 0xE0000000
#define LFB_VIRTUAL  0x300000
#define PAGE_SIZE    0x1000

/* render rectangle in 32 bpp modes. */
void rect32(int x, int y, int w, int h, int col) {
	uint32_t* lfb = (uint32_t*)LFB_VIRTUAL;
	for (uint32_t k = 0; k < h; k++)
		for (uint32_t j = 0; j < w; j++)
			lfb[(j + x) + (k + y) * WIDTH] = col;
}

/* thread cycles through colors of red. */
void kthread_1() {
	int col = 0;
	bool dir = true;
	while (1) {
		rect32(200, 250, 100, 100, col << 16);
		if (dir) {
			if (col++ == 0xfe)
				dir = false;
		}
		else
			if (col-- == 1)
				dir = true;
	}
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


	Process* pProcess = ProcessManager::GetInstance()->CreateMemoryProcess(kthread_1);

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
void cmd_read() {

	//! get pathname
	char path[32];
	DebugPrintf("\n\rex: \"file.txt\", \"a:\\file.txt\", \"a:\\folder\\file.txt\"\n\rFilename> ");
	console.GetCommand(path, 30);

	//! open file
	FILE file = volOpenFile(path);

	//! test for invalid file
	if (file.flags == FS_INVALID) {

		DebugPrintf("\n\rUnable to open file");
		return;
	}

	//! cant display directories
	if ((file.flags & FS_DIRECTORY) == FS_DIRECTORY) {

		DebugPrintf("\n\rUnable to display contents of directory.");
		return;
	}

	//! top line
	DebugPrintf("\n\n\r-------[%s]-------\n\r", file.name);

	//! display file contents
	while (file.eof != 1) {

		//! read cluster
		unsigned char buf[512];
		volReadFile(&file, buf, 512);

		//! display file
		for (int i = 0; i<512; i++)
			DebugPutc(buf[i]);

		//! wait for input to continue if not EOF
		if (file.eof != 1) {
			DebugPrintf("\n\r------[Press a key to continue]------");
			console.GetChar();
			DebugPrintf("\r"); //clear last line
		}
	}

	//! done :)
	DebugPrintf("\n\n\r--------[EOF]--------");
}

void go_user() {

	int stack = 0;
	_asm mov[stack], esp

	extern void tss_set_stack(uint16_t, uint16_t);
	tss_set_stack(0x10, (uint16_t)stack & 0xffff);

	enter_usermode();

	char* testStr = "\n\rWe are inside of your computer...";

	//! call OS-print message
	_asm xor eax, eax
	_asm lea ebx, [testStr]
		_asm int 0x80

	//! cant do CLI+HLT here, so loop instead
	while (1);
}

// proc (process) command
void cmd_proc() {

	int ret = 0;
	char name[32];

	DebugPrintf("\n\rProgram file: ");
	console.GetCommand(name, 30);

	Process* pProcess = ProcessManager::GetInstance()->CreateProcess(name, PROCESS_USER);
	if (pProcess == 0)
	{
		DebugPrintf("\n\rError creating process");
	}
	else
		ProcessManager::GetInstance()->ExecuteProcess(pProcess);
}

bool ConsoleManager::RunCommand(char* buf) 
{

	if (strcmp(buf, "user") == 0) {
		go_user();
	}

	//! exit command
	if (strcmp(buf, "exit") == 0) {
		return true;
	}

	//! clear screen
	else if (strcmp(buf, "cls") == 0) {
		console.Clear();
	}

	//! help
	else if (strcmp(buf, "help") == 0) {

		console.Print("Orange OS Console Help\n");		
		console.Print(" - exit: quits and halts the system\n");
		console.Print(" - cls: clears the display\n");
		console.Print(" - help: displays this message\n");
		console.Print(" - read: reads a file\n");
		console.Print(" - reset: Resets and recalibrates floppy for reading\n");
		console.Print(" - proc: Run process\n");		
	}

	//! read sector
	else if (strcmp(buf, "read") == 0) {
		cmd_read();
	}
	else if (strcmp(buf, "memstate") == 0) {
		cmd_memstate();
	}
	else if (strcmp(buf, "alloc") == 0) {
		cmd_alloc();
	}
	else if (strcmp(buf, "memtask") == 0) {
		cmd_memtask();
	}

	//! run process
	else if (strcmp(buf, "proc") == 0) {
		cmd_proc();
	}
	else
	{
		console.Print("Unknown Command\n");
	}

	return false;
}