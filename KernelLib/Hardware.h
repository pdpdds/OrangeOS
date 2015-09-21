#pragma once
#include "windef.h"


#define TIMEOUT_PER_SECOND		50				/* system timer (irq0) */

bool HalpEnableA20(void);
bool HalpInitPIC(void);
bool HalpInitSysTimer(BYTE timeoutPerSecond);
bool HalpStartIntService(void);
bool HalpSetupGDT(void);

