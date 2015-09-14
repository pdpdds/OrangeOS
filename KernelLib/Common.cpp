#include "Common.h"
extern "C" int _outp(unsigned short, int);
extern "C" int _outl(unsigned short, int);
extern "C" unsigned short _outpw(unsigned short, unsigned short);
extern "C" int _inp(unsigned short);
extern "C" unsigned short _inpw(unsigned short);
#pragma intrinsic(_inp, _inpw, _outp, _outpw)

#define OutPortByte(prt, val) _outp(prt, val)
#define OutPortWord(prt, val) _outpw(prt, val)
#define OutPortLong(prt, val) _outl(prt, val)
#define InportByte(prt) _inp(prt)
#define InportWord(prt) _inpw(prt)

void outportByte(ushort port, uchar value)
{
	OutPortByte(port, value);
}

void outportWord(ushort port, ushort value)
{
	OutPortWord(port, value);
}

uchar inportByte(ushort port)
{
	return (uchar)InportByte(port);
}

ushort inportWord(ushort port)
{
	return InportWord(port);
}

extern "C" void __cxa_pure_virtual()
{
	//This doesn't need to have an implementation. If a virtual call cannot be made, nothing needs to be done
}

//These methods will require an implementation when you implement a memory manager
/*void *operator new( size_t size)
{
	return (void *)0;
}

void *operator new[](size_t size)
{
	return (void *)0;
}

void operator delete(void *p)
{
}

void operator delete[](void *p)
{
}
*/