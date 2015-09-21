#include "Common.h"

void OutPortByte(ushort port, uchar value)
{
	_outp(port, value);
}

void OutPortWord(ushort port, ushort value)
{
	_outpw(port, value);
}

uchar InPortByte(ushort port)
{
	return (uchar)_inp(port);
}

ushort InPortWord(ushort port)
{
	return _inpw(port);
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