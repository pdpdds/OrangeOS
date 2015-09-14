#pragma once
#include "winDef.h"
#include "sysDesc.h"

//! maximum amount of descriptors allowed
#define MAX_DESCRIPTORS					6

/***	 gdt descriptor access bit flags.	***/

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


#pragma pack (push, 1)

//GDT 디스크립터 표현
typedef struct tag_gdtDescriptor 
{
	USHORT			segmentLimit;
	USHORT			baseLow;
	BYTE			baseMiddle;

//디스크립터 플래그
	BYTE			flags;
	BYTE			grand;
	BYTE			baseHigh;
}gdtDescriptor;

//GDT의 위치 및 크기를 알려줄 수 있는 구조체
//이값을 통해 GDTR 레지스터는 GDT를 가리키게 되며
//프로세서는 GDTR 레지스터를 통해 GDT에 접근한다.
typedef struct tag_gdtr {
	USHORT		limit; //GDT의 크기
	UINT		base; //GDT의 시작 주소
}gdtr;

#pragma pack (pop)

class GDT
{
public:
	GDT(void);
	virtual ~GDT(void);
	
	static bool InitializeGDT();

private:
	static void SetDescriptor(UINT i, UINT64 base, UINT64 limit, BYTE access, BYTE grand);
	static gdtDescriptor* GetDescriptor(int index);
	static bool WriteTssIntoGdt(TSS_32 *pTss32, DWORD TssSize, DWORD TssNumber, bool SetBusy);
	static VOID ChangeTssBusyBit(WORD TssSeg, bool SetBit);

	static void InstallGDT ();
	static  gdtDescriptor	m_gdt [MAX_DESCRIPTORS];
	static gdtr			m_gdtr;
};