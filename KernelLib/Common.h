#pragma once
#include "windef.h"

extern "C" int _outp(unsigned short, int);
extern "C" int _outl(unsigned short, int);
extern "C" unsigned short _outpw(unsigned short, unsigned short);
extern "C" int _inp(unsigned short);
extern "C" unsigned short _inpw(unsigned short);
#pragma intrinsic(_inp, _inpw, _outp, _outpw)

typedef unsigned long  ulong;			//64 bit integer
typedef unsigned int   uint;			//32 bit integer
typedef unsigned short ushort;			//16 bit integer
typedef unsigned char  uchar;			//8 bit integer

inline static void enable()
{
	__asm
	{
		sti
	}
}

inline static void disable()
{
	__asm
	{
		cli
	}
}

struct multibootInfo
{
	uint flags;
	uint memoryLow;
	uint memoryHigh;
	uint bootDevice;
	uint commandLine;
	uint moduleCount;
	uint moduleAddresses;
	uint syms0;
	uint syms1;
	uint syms2;
	uint memoryMapLength;
	uint memoryMapAddress;
	uint drivesLength;
	uint drivesAddress;
	uint configTable;
	uint bootloaderName;
	uint apmTable;
	uint vbeControlInformation;
	uint vbeModeInformation;
	ushort vbeMode;
	uint vbeInterfaceAddress;
	ushort vbeInterfaceLength;
};

/* The Multiboot header.  */
typedef struct multiboot_header
{
  unsigned long magic;
  unsigned long flags;
  unsigned long checksum;
  unsigned long header_addr;
  unsigned long load_addr;
  unsigned long load_end_addr;
  unsigned long bss_end_addr;
  unsigned long entry_addr;
} multiboot_header_t;

void outportByte(ushort port, uchar value);
void outportWord(ushort port, ushort value);

uchar inportByte(ushort port);
ushort inportWord(ushort port);
ulong inportLong(ushort port);
/*
void *operator new ( size_t size);
void *operator new[] ( size_t size);
void operator delete (void *p);
void operator delete[] (void *p);

inline static void enable()
{
	__asm 
	{
		sti
	}
}

inline static void disable()
{
	__asm 
	{
		cli
	}
}
*/
