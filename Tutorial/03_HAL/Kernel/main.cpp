﻿#include "header.h"
#include "DebugDisplay.h"
#include "string.h"
#include "Console.h"
#include "HAL.h"
#include "RTC.H"
#include "IDT.h"
#include "GDT.h"
#include "PIT.h"
#include "PIC.h"
#include "exception.h"
#include "main.h"

bool systemOn = false;

extern uint32_t g_kernelSize;
extern void __cdecl  InitializeConstructors();

void SetInterruptVector();

__declspec(naked) void multiboot_entry(void)
{
	__asm {
		align 4

		multiboot_header:
		dd(MULTIBOOT_HEADER_MAGIC); magic number
			dd(MULTIBOOT_HEADER_FLAGS); flags
			dd(CHECKSUM); checksum
			dd(HEADER_ADRESS); header address
			dd(LOADBASE); load address
			dd(00); load end address : not used
			dd(00); bss end addr : not used
			dd(HEADER_ADRESS + 0x20); entry_addr: equ kernel entry
			; 0x20 is the size of multiboot header

			kernel_entry :
		mov     esp, KERNEL_STACK; Setup the stack

			push    0; Reset EFLAGS
			popf

			push    ebx; Push multiboot info address
			push    eax; and magic number
			; which are loaded in registers
			; eax and ebx before jump to
			; entry adress
			; [HEADER_ADRESS + 0x20]
			call    main; kernel entry
			halt :
		jmp halt; halt processor

	}
}

void main(unsigned long magic, unsigned long addr){
//int _cdecl kmain(multiboot_info* bootinfo) {
	Console console;
	InitializeConstructors();
	InterruptDisable();

	i86_gdt_initialize();
	i86_idt_initialize(0x8);
	i86_pic_initialize(0x20, 0x28);
	i86_pit_initialize();

	SetInterruptVector();
	
	i86_pit_start_counter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
	
	InterruptEnable();	

//커널사이즈에 512를 곱하면 바이트로 환산된다.
//현재 커널의 사이즈는 4096바이트다.
	g_kernelSize *= 512;
	
	console.SetBackColor(ConsoleColor::Blue);
	console.Clear();

	console.Print("Orange OS Console System Initialize\n");
	console.Print("KernelSize : %d Bytes\n", g_kernelSize);

	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.
	//주소값을 표기해야 하므로 %x를 사용한다.
	console.Print("systemOn Variable Address : 0x%x\n", &systemOn);

	char str[256];
	memset(str, 0, 256);

	//int j = 0;
	//int k = 50 / j;

	for (;;);

	//return 0;
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