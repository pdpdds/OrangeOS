
//****************************************************************************
//**
//**    gdt.cpp
//**
//**	global descriptor table (gdt) for i86 processors. This handles
//**	the basic memory map for the system and permission levels
//**
//****************************************************************************

//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================

#include "gdt.h"
#include <string.h>
#include "windef.h"
#include "../kernel/task.h"
#include "defines.h"

//============================================================================
//    IMPLEMENTATION PRIVATE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE STRUCTURES / UTILITY CLASSES
//============================================================================

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

//! processor gdtr register points to base of gdt. This helps
//! us set up the pointer
struct gdtr {

	//! size of gdt
	uint16_t		m_limit;

	//! base address of gdt
	uint32_t		m_base;
};

#ifdef _MSC_VER
#pragma pack (pop, 1)
#endif

//============================================================================
//    IMPLEMENTATION REQUIRED EXTERNAL REFERENCES (AVOID)
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE DATA
//============================================================================

//! global descriptor table is an array of descriptors
static struct gdt_descriptor	_gdt [MAX_DESCRIPTORS];

//! gdtr data
static struct gdtr				_gdtr;

//============================================================================
//    INTERFACE DATA
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTION PROTOTYPES
//============================================================================

//! install gdtr
static void gdt_install ();

//============================================================================
//    IMPLEMENTATION PRIVATE FUNCTIONS
//============================================================================

//! install gdtr
static void gdt_install () {
#ifdef _MSC_VER
	_asm lgdt [_gdtr]
#endif
}

//============================================================================
//    INTERFACE FUNCTIONS
//============================================================================

VOID HalChangeTssBusyBit(WORD TssSeg, BOOL SetBit)
{
	ENTER_CRITICAL_SECTION();
	if (SetBit) {
		_gdt[TssSeg >> 3].flags |= 0x02;
	}
	else {
		_gdt[TssSeg >> 3].flags &= 0xfd;
	}
	EXIT_CRITICAL_SECTION();
}
BOOL HalSetupTaskLink(TSS_32 *pTss32, WORD TaskLink)
{
	ENTER_CRITICAL_SECTION();
	pTss32->eflags |= 0x4000;
	pTss32->prev_task_link = TaskLink;
	EXIT_CRITICAL_SECTION();

	return TRUE;
}

BOOL HalWriteTssIntoGdt(TSS_32 *pTss32, DWORD TssSize, DWORD TssNumber, BOOL SetBusy)
{
	/* todo : support TssSize parameter */
	ENTER_CRITICAL_SECTION();
	memset((void*)&_gdt[TssNumber >> 3], 0, sizeof(gdt_descriptor));
	_gdt[TssNumber >> 3].limit = (BYTE)(TssSize & 0x0000ffff);
	_gdt[TssNumber >> 3].grand = (BYTE)((TssSize & 0x000f0000) >> 16);
	_gdt[TssNumber >> 3].baseLo = (BYTE)(((int)pTss32) & 0x0000ffff);
	_gdt[TssNumber >> 3].baseMid = (BYTE)((((int)pTss32) & 0x00ff0000) >> 16);
	_gdt[TssNumber >> 3].baseHi = (BYTE)((((int)pTss32) & 0xff000000) >> 24);
	_gdt[TssNumber >> 3].flags = (SetBusy ? 0x8b : 0x89);
	EXIT_CRITICAL_SECTION();

	return TRUE;
}

//! Setup a descriptor in the Global Descriptor Table
void gdt_set_descriptor(uint32_t i, uint64_t base, uint64_t limit, uint8_t access, uint8_t grand)
{
	if (i > MAX_DESCRIPTORS)
		return;

	//! null out the descriptor
	memset ((void*)&_gdt[i], 0, sizeof (gdt_descriptor));

	//! set limit and base addresses
	_gdt[i].baseLo	= uint16_t(base & 0xffff);
	_gdt[i].baseMid	= uint8_t((base >> 16) & 0xff);
	_gdt[i].baseHi	= uint8_t((base >> 24) & 0xff);
	_gdt[i].limit	= uint16_t(limit & 0xffff);

	//! set flags and grandularity bytes
	_gdt[i].flags = access;
	_gdt[i].grand = uint8_t((limit >> 16) & 0x0f);
	_gdt[i].grand |= grand & 0xf0;
}


//! returns descriptor in gdt
gdt_descriptor* i86_gdt_get_descriptor (int i) {

	if (i > MAX_DESCRIPTORS)
		return 0;

	return &_gdt[i];
}


//! initialize gdt
int i86_gdt_initialize () {

	//! set up gdtr
	_gdtr.m_limit = (sizeof (struct gdt_descriptor) * MAX_DESCRIPTORS)-1;
	_gdtr.m_base = (uint32_t)&_gdt[0];

	//! set null descriptor
	gdt_set_descriptor(0, 0, 0, 0, 0);

	//! set default code descriptor
	gdt_set_descriptor (1,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_EXEC_CODE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//! set default data descriptor
	gdt_set_descriptor (2,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//! set default user mode code descriptor
	gdt_set_descriptor (3,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_EXEC_CODE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY|I86_GDT_DESC_DPL,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//! set default user mode data descriptor
	gdt_set_descriptor (4,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY|I86_GDT_DESC_DPL,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//! install gdtr
	gdt_install ();

	return 0;
}

//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================
//****************************************************************************
//**
//**    END[gdt.cpp]
//**
//****************************************************************************
