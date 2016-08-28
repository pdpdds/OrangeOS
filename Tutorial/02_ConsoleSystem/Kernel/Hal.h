#pragma once
#include "windef.h"

extern "C" int _outp(unsigned short, int);
extern "C" int _outl(unsigned short, int);
extern "C" unsigned short _outpw(unsigned short, unsigned short);
extern "C" int _inp(unsigned short);
extern "C" unsigned short _inpw(unsigned short);

void OutPortByte(ushort port, uchar value);
void OutPortWord(ushort port, ushort value);
uchar InPortByte(ushort port);
ushort InPortWord(ushort port);

inline static void InterruptEnable()
{
	__asm
	{
		sti
	}
}

inline static void InterruptDisable()
{
	__asm
	{
		cli
	}
}