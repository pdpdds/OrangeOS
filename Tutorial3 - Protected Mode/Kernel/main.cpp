/*
======================================
main.cpp
-kernel startup code
======================================
*/
#include "Console.h"
#include "string.h"
#include "RTC.h"
#include "GDT.h"
#include "sprintf.h"
#include "Hardware.h"
#include "bootinfo.h"
#include "KeyboardController.h"
#include "PIT.h"
#include "IDT.h"
#include "kybrd.h"
#include "IStream.h"
#include "HardDisk.h"

void Halt();
void DbgError(char* pMsg);
void WelComeMessage();
void InitialzeHadware();

Console console;
char* g_str = "address test";
int g_var = 0;
PIT PITTimer(0);
IDT idt;

#include "Hal.h"


using namespace std;
void _cdecl main() 
{	
	WelComeMessage();
	HalpSetupGDT();
	idt.InitializeIDT(0x8);

	hal_initialize();

	//KeyboardController::SetupInterrupts();
	//KeyboardController::SetLEDs(true, true, true);

	//PITTimer.Enable(50);	

	HardDiskHandler aa;
	aa.Initialize();
	enable();
	//HalpStartIntService();
	kkybrd_install(33);

	WelComeMessage();

	KeyboardController::SetLEDs(true, true, true);
	
	Halt();
}

void DbgError(char* pMsg)
{
	console.WriteString(pMsg, White, Black);
	while (1);
}

void Halt()
{
	for (;;);
}

void WelComeMessage()
{	
	console.Clear();
	char* szWelcome = "Welcome to Orange OS!!";

	char szBuf[256];
	strcpy(szBuf, szWelcome);

	console.WriteString(szBuf, White, Black);
	console.WriteString("This is a C++ OS.", White, Black);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	memset(szBuf, 0, 256);

	sprintf(szBuf, "LocalTime : %d/%d %d:%d", (int)sysTime.wMonth, (int)sysTime.wDay, (int)sysTime.wHour, (int)sysTime.wMinute);
	console.WriteString(szBuf, White, Black);

	sprintf(szBuf, "Address : %x", &g_var);

	console.WriteString(szBuf, White, Black);	
}

void InitialzeHadware()
{
	if (!HalpEnableA20())
	{
		DbgError("HalpEnableA20() returned an error");
	}
	if (!HalpInitPIC()) {
		DbgError("HalpInitPIC() returned an error");
	}
	if (!HalpInitSysTimer(TIMEOUT_PER_SECOND)) {
		DbgError("HalpInitSysTimer() returned an error");
	}
}