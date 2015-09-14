#include "GDT.h"
#include "string.h"

gdtDescriptor	GDT::m_gdt [MAX_DESCRIPTORS];
gdtr			GDT::m_gdtr;

GDT::GDT(void)
{
}

GDT::~GDT(void)
{
}

bool GDT::InitializeGDT () {

	//! set up gdtr
	m_gdtr.limit = (sizeof (gdtDescriptor) * MAX_DESCRIPTORS)-1;
	m_gdtr.base = (UINT)&m_gdt[0];

	//! set null descriptor
	SetDescriptor(0, 0, 0, 0, 0);

	//! set default code descriptor
	SetDescriptor (1,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_EXEC_CODE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//! set default data descriptor
	SetDescriptor (2,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);


	//! set default user mode code descriptor
	SetDescriptor (3,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_EXEC_CODE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY|I86_GDT_DESC_DPL,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);

	//! set default user mode data descriptor
	SetDescriptor (4,0,0xffffffff,
		I86_GDT_DESC_READWRITE|I86_GDT_DESC_CODEDATA|I86_GDT_DESC_MEMORY|I86_GDT_DESC_DPL,
		I86_GDT_GRAND_4K | I86_GDT_GRAND_32BIT | I86_GDT_GRAND_LIMITHI_MASK);


	//! install gdtr
	InstallGDT ();

	return true;
}

//! Setup a descriptor in the Global Descriptor Table
void GDT::SetDescriptor(UINT index, UINT64 base, UINT64 limit, BYTE access, BYTE grand)
{
	if (index > MAX_DESCRIPTORS)
		return;

	//! null out the descriptor
	memset ((void*)&m_gdt[index], 0, sizeof (gdtDescriptor));

	//! set limit and base addresses
	m_gdt[index].baseLow	= USHORT(base & 0xffff);
	m_gdt[index].baseMiddle	= BYTE((base >> 16) & 0xff);
	m_gdt[index].baseHigh	= BYTE((base >> 24) & 0xff);
	m_gdt[index].segmentLimit	= USHORT(limit & 0xffff);

	//! set flags and grandularity bytes
	m_gdt[index].flags = access;
	m_gdt[index].grand = BYTE((limit >> 16) & 0x0f);
	m_gdt[index].grand |= grand & 0xf0;
}


//! returns descriptor in gdt
gdtDescriptor* GDT::GetDescriptor (int index) {

	if (index > MAX_DESCRIPTORS)
		return 0;

	return &m_gdt[index];
}

//! install gdtr
void GDT::InstallGDT () 
{
	_asm lgdt [m_gdtr]
}

bool GDT::WriteTssIntoGdt(TSS_32 *pTss32, DWORD TssSize, DWORD TssNumber, bool SetBusy)
{
	 /*todo : support TssSize parameter */
ENTER_CRITICAL_SECTION();
	m_gdt[TssNumber>>3].segmentLimit	=  (USHORT)(TssSize & 0x0000ffff);
	m_gdt[TssNumber>>3].grand		= (BYTE)((TssSize & 0x000f0000) >> 16);
	m_gdt[TssNumber>>3].baseLow			=  (USHORT)(((int)pTss32) & 0x0000ffff);
	m_gdt[TssNumber>>3].baseMiddle			= (BYTE)((((int)pTss32) & 0x00ff0000) >> 16);
	m_gdt[TssNumber>>3].baseHigh			= (BYTE)((((int)pTss32) & 0xff000000) >> 24);
	m_gdt[TssNumber>>3].flags			= (SetBusy ? 0x8b : 0x89);
EXIT_CRITICAL_SECTION();

/*
ENTER_CRITICAL_SECTION();
	m_GdtTable[TssNumber>>3].limit_1		=  (BYTE)(TssSize & 0x000000ff);
	m_GdtTable[TssNumber>>3].limit_2		= (BYTE)((TssSize & 0x0000ff00) >> 8);
	m_GdtTable[TssNumber>>3].glimit_3		= (BYTE)((TssSize & 0x000f0000) >> 16);
	m_GdtTable[TssNumber>>3].base_1			=  (BYTE)(((int)pTss32) & 0x000000ff);
	m_GdtTable[TssNumber>>3].base_2			= (BYTE)((((int)pTss32) & 0x0000ff00) >> 8);
	m_GdtTable[TssNumber>>3].base_3			= (BYTE)((((int)pTss32) & 0x00ff0000) >> 16);
	m_GdtTable[TssNumber>>3].base_4			= (BYTE)((((int)pTss32) & 0xff000000) >> 24);
	m_GdtTable[TssNumber>>3].type			= (SetBusy ? 0x8b : 0x89);
	EXIT_CRITICAL_SECTION();
*/
	return TRUE;
}


VOID GDT::ChangeTssBusyBit(WORD TssSeg, bool SetBit)
{
ENTER_CRITICAL_SECTION();
	if(SetBit) {
		m_gdt[TssSeg>>3].flags |= 0x02;
	} else {
		m_gdt[TssSeg>>3].flags &= 0xfd;
	}
EXIT_CRITICAL_SECTION();
}
