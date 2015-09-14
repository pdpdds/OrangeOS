#pragma once
#include "windef.h"

extern "C" int _outp(unsigned short, int);
extern "C" int _outl(unsigned short, int);
#define OutPortByte(prt, val) _outp(prt, val)
#define OutPortWord(prt, val) _outpw(prt, val)
#define InportByte(prt) _inp(prt)
#define InportWord(prt) _inpw(prt)

typedef unsigned char UCHAR;
#define TIMEOUT_PER_SECOND		50				/* system timer (irq0) */

bool HalpEnableA20(void);
bool HalpInitPIC(void);
bool HalpInitSysTimer(BYTE timeoutPerSecond);
bool HalpStartIntService(void);
bool HalpSetupGDT(void);

