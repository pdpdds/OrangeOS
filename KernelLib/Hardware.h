#pragma once
#include "windef.h"

extern "C" int _outp(unsigned short, int);
extern "C" int _outl(unsigned short, int);
#define OutPortByte(prt, val) _outp(prt, val)
#define OutPortWord(prt, val) _outpw(prt, val)
typedef unsigned char UCHAR;
#define TIMEOUT_PER_SECOND		50				/* system timer (irq0) */

bool HalpInitPIC(void);
bool HalpInitSysTimer(BYTE timeoutPerSecond);

