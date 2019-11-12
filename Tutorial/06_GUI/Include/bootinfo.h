#pragma once
#include <stdint.h>
#include "windef.h"

//! multiboot info structure passed from boot loader
struct multiboot_info {

	uint32_t	m_flags;
//	uint64_t	m_memorySize;
	
	uint32_t	m_memoryLo;
	uint32_t	m_memoryHi;
	uint32_t	m_bootDevice;
	uint32_t	m_cmdLine;
	uint32_t	m_modsCount;
	uint32_t	m_modsAddr;
	uint32_t	m_syms0;
	uint32_t	m_syms1;
	uint32_t	m_syms2;
	uint32_t	m_mmap_length;
	uint32_t	m_mmap_addr;
	uint32_t	m_drives_length;
	uint32_t	m_drives_addr;
	uint32_t	m_config_table;
	uint32_t	m_bootloader_name;
	uint32_t	m_apm_table;
	uint32_t	m_vbe_control_info;
	uint32_t	m_vbe_mode_info;
	uint16_t	m_vbe_mode;
	uint32_t	m_vbe_interface_addr;
	uint16_t	m_vbe_interface_len;
};

struct VbeModeInfo
{
	UINT16 ModeAttributes;
	char WinAAttributes;
	char WinBAttributes;
	UINT16 WinGranularity;
	UINT16 WinSize;
	UINT16 WinASegment;
	UINT16 WinBSegment;
	UINT32 WinFuncPtr;
	short BytesPerScanLine;
	short XRes;
	short YRes;
	char XCharSize;
	char YCharSize;
	char NumberOfPlanes;
	char BitsPerPixel;
	char NumberOfBanks;
	char MemoryModel;
	char BankSize;
	char NumberOfImagePages;
	char res1;
	char RedMaskSize;
	char RedFieldPosition;
	char GreenMaskSize;
	char GreenFieldPosition;
	char BlueMaskSize;
	char BlueFieldPosition;
	char RsvedMaskSize;
	char RsvedFieldPosition;
	char DirectColorModeInfo; //MISSED IN THIS TUTORIAL!! SEE ABOVE
							  //VBE 2.0
	UINT32 PhysBasePtr;
	UINT32 OffScreenMemOffset;
	short OffScreenMemSize;
	//VBE 2.1
	short LinbytesPerScanLine;
	char BankNumberOfImagePages;
	char LinNumberOfImagePages;
	char LinRedMaskSize;
	char LinRedFieldPosition;
	char LingreenMaskSize;
	char LinGreenFieldPosition;
	char LinBlueMaskSize;
	char LinBlueFieldPosition;
	char LinRsvdMaskSize;
	char LinRsvdFieldPosition;
	char res2[194];
};