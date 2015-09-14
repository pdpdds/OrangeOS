#pragma once
#include "windef.h"

//! i86 defines 256 possible interrupt handlers (0-255)
#define I86_MAX_INTERRUPTS		256

//! must be in the format 0D110, where D is descriptor type
#define I86_IDT_DESC_BIT16		0x06	//00000110
#define I86_IDT_DESC_BIT32		0x0E	//00001110
#define I86_IDT_DESC_RING1		0x40	//01000000
#define I86_IDT_DESC_RING2		0x20	//00100000
#define I86_IDT_DESC_RING3		0x60	//01100000
#define I86_IDT_DESC_PRESENT	0x80	//10000000

#pragma pack (push, 1)

typedef struct tag_idtDescriptor 
{

	USHORT		offsetLow;
	USHORT		selector;
	BYTE		reserved;
	BYTE		flags;
	USHORT		offsetHigh;
}idtDescriptor;

typedef struct tag_idtr 
{
	USHORT		limit;
	UINT		base;
}idtr;

#pragma pack (pop)

class IDT
{
public:
	IDT(void);
	virtual ~IDT(void);

	idtDescriptor* GetDescriptor (UINT index);
	int InitializeIDT(USHORT codeSelector);
	void SetISR(int index, I86_IRQ_HANDLER irq, int flags = 0);

	static idtDescriptor	m_idt [I86_MAX_INTERRUPTS];
	static idtr		m_idtr;

	void InstalllDT();

	static int InstallISR(int index, USHORT flags, USHORT selector, I86_IRQ_HANDLER irq);
};
