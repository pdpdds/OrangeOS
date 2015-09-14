#include "IDT.h"
#include "string.h"
#include "Console.h"
#include "Exception.h"
#include "Hal.h"

extern void *memset(char *dest, int c, size_t count);

idtDescriptor	IDT::m_idt[I86_MAX_INTERRUPTS];
idtr			IDT::m_idtr;

extern Console console;

//시스템이 다루지 못한 예외 핸들러를 다룰 수 있는 기본 핸들러
static void i86_default_handler () {

//예외를 처리하는 도중에 새로운 예외를 CPU가 처리하는 것을 막도록 함
	_asm cli

	//! print debug message and halt
#ifdef _DEBUG
	DebugClrScr (0x18);
	DebugGotoXY (0,0);
	DebugSetColor (0x1e);
	DebugPrintf ("*** [i86 Hal] i86_default_handler: Unhandled Exception");
#endif

	console.WriteString("dsfsdsdds", White, Black);
	for(;;);
}

IDT::IDT(void)
{
}

IDT::~IDT(void)
{
}

idtDescriptor* IDT::GetDescriptor (UINT index)
{
	if (index > I86_MAX_INTERRUPTS)
		return 0;

	return &m_idt[index];
}

int IDT::InstallISR(int index, USHORT flags, USHORT selector, I86_IRQ_HANDLER irq)
{
	if (index > I86_MAX_INTERRUPTS)
		return 0;

	if (!irq)
		return 0;

	//! get base address of interrupt handler
	UINT64	uiBase = (UINT64)&(*irq);

	//! store base address into idt
	m_idt[index].offsetLow		=	USHORT(uiBase & 0xffff);
	m_idt[index].offsetHigh		=	USHORT((uiBase >> 16) & 0xffff);
	m_idt[index].reserved	=	0;
	m_idt[index].flags		=	BYTE(flags);
	m_idt[index].selector		=	selector;

	return	0;
}

int IDT::InitializeIDT(USHORT codeSelector)
{	
	m_idtr.limit = sizeof(idtDescriptor) * I86_MAX_INTERRUPTS -1;
	m_idtr.base	= (UINT)&m_idt[0];

	memset ((char*)&m_idt[0], 0, sizeof (idtDescriptor) * I86_MAX_INTERRUPTS-1);

	//디폴트 핸들러 등록
	for (int i=20; i<I86_MAX_INTERRUPTS; i++)
		InstallISR (i, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32, codeSelector, (I86_IRQ_HANDLER)i86_default_handler);

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

	InstalllDT();

	return 0;
}

void IDT::InstalllDT()
{
	_asm lidt [m_idtr]
}

void IDT::SetISR(int index, I86_IRQ_HANDLER irq, int flags)
{
	InstallISR(index, I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | flags, 0x8, irq);
}

