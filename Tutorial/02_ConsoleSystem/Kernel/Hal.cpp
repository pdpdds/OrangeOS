#include "Hal.h"

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