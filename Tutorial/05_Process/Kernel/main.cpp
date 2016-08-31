#include "header.h"
#include "DebugDisplay.h"
#include "string.h"
#include "sprintf.h"
#include "Console.h"
#include "HAL.h"
#include "RTC.H"
#include "IDT.h"
#include "GDT.h"
#include "PIT.h"
#include "PIC.h"
#include "exception.h"
#include "mmngr_phys.h"
#include "mmngr_virtual.h"
#include "kheap.h"
#include "HardDisk.h"
#include "flpydsk.h"
#include "fat12.h"
#include "ZetPlane.h"
#include "sysapi.h"
#include "tss.h"
#include "ProcessManager.h"
#include "ConsoleManager.h"
#include "List.h"
#include "kybrd.h"


bool systemOn = false;
Console console;
extern uint32_t g_kernelSize;

void SetInterruptVector();
bool InitializeMemorySystem(multiboot_info* bootinfo, uint32_t kernelSize);
void CreateKernelHeap(int kernelSize);
void InitializeFloppyDrive();

int _cdecl kmain(multiboot_info* bootinfo) {

	InterruptDisable();

	i86_gdt_initialize();
	i86_idt_initialize(0x8);
	i86_pic_initialize(0x20, 0x28);
	i86_pit_initialize();
	
	i86_pit_start_counter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
	
	InterruptEnable();

	SetInterruptVector();

	InitializeMemorySystem(bootinfo, g_kernelSize);

//커널사이즈에 512를 곱하면 바이트로 환산된다.
//현재 커널의 사이즈는 4096바이트다.
	g_kernelSize *= 512;

	//! install the keyboard to IR 33, uses IRQ 1
	kkybrd_install(33);

	InitializeFloppyDrive();

	//HardDiskHandler hardHandler;
	//hardHandler.Initialize();	

	
	console.SetBackColor(ConsoleColor::Blue);
	console.Clear();

	//console.Print("HardDisk Count : %d\n", hardHandler.GetTotalDevices());
	console.Print("Orange OS Console System Initialize\n");
	console.Print("KernelSize : %d Bytes\n", g_kernelSize);

	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.
	//주소값을 표기해야 하므로 %x를 사용한다.
	console.Print("systemOn Variable Address : 0x%x\n", &systemOn);

	char str[256];
	memset(str, 0, 256);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	sprintf(str, "LocalTime : %d %d/%d %d:%d %d\n", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	console.Print("%s", str);

	//파라메터 kernelSize는 스택에 존재한다. stack의 베이스 주소는 0x9000이다.
	sprintf(str, "Parameter kernelSize Variable Address : %x\n", (unsigned int)&g_kernelSize);
	console.Print("%s", str);

	ZetPlane* pZetPlane = new ZetPlane();
	pZetPlane->m_rotation = 50;
	console.Print("ZetPlane's Rotation is %d\n", pZetPlane->m_rotation);

	InitializeSysCall();

	//! initialize TSS
	install_tss(5, 0x10, 0x9000);

	Process* pProcess = ProcessManager::GetInstance()->CreateSystemProcess();

	if(pProcess)	
		console.Print("Create Success System Process\n");

	ProcessManager::GetInstance()->g_pCurProcess = pProcess;
	systemOn = true;

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


	int j = 0;
	int k = 50 / j;

	return 0;
}

void SetInterruptVector()
{
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

	//i86_install_ir(SYSTEM_TMR_INT_NUMBER, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | 0x0500, 0x8, (I86_IRQ_HANDLER)TMR_TSS_SEG);
}

bool InitializeMemorySystem(multiboot_info* bootinfo, uint32_t kernelSize)
{
	pmmngr_init((size_t)bootinfo->m_memorySize, 0xC0000000 + kernelSize * 512);

	memory_region*	region = (memory_region*)0x1000;

	for (int i = 0; i<10; ++i) {

		if (region[i].type>4)
			break;

		if (i>0 && region[i].startLo == 0)
			break;

		pmmngr_init_region(region[i].startLo, region[i].sizeLo);
	}
	pmmngr_deinit_region(0x100000, kernelSize * 512 + 4096 * 1024 + 4096 * 1024);
	/*
	kernel stack location
	*/
	pmmngr_deinit_region(0x0, 0x10000);

	//! initialize our vmm
	vmmngr_initialize();

	CreateKernelHeap(kernelSize);

	return true;
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


void InitializeFloppyDrive()
{
	//! set drive 0 as current drive
	flpydsk_set_working_drive(0);

	//! install floppy disk to IR 38, uses IRQ 6
	flpydsk_install(38);

	//! initialize FAT12 filesystem
	fsysFatInitialize();
}

void run() 
{
	ConsoleManager manager;
	
	char	commandBuffer[MAXPATH];

	while (1) 
	{
		console.Print("Command> ");
		
		console.GetCommand(commandBuffer, MAXPATH-2);
		console.Print("\n");
		
		if (manager.RunCommand(commandBuffer) == true)
			break;
	}
}
