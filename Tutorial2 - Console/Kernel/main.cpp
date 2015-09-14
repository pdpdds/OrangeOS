/*
======================================
main.cpp
-kernel startup code
======================================
*/
#include "Console.h"
#include "string.h"
#include "RTC.h"
#include "sprintf.h"
#include "Hardware.h"

void Halt();
void DbgError(char* pMsg);

Console console;

void _cdecl main() {

	console.Clear();
	char* szWelcome = "Welcome to Orange OS!!";

	if (!HalpInitPIC()) {
		DbgError("HalpInitPIC() returned an error");
	}
	if (!HalpInitSysTimer(TIMEOUT_PER_SECOND)) {
		DbgError("HalpInitSysTimer() returned an error");
	}

	char szBuf[256];
	strcpy(szBuf, szWelcome);

	console.WriteString(szBuf, White, Black);
	console.WriteString("This is a C++ OS.", White, Black);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	memset(szBuf, 0, 256);

	sprintf(szBuf, "LocalTime : %d/%d %d:%d", (int)sysTime.wMonth, (int)sysTime.wDay, (int)sysTime.wHour, (int)sysTime.wMinute);

	console.WriteString(szBuf, White, Black);	
}

void DbgError(char* pMsg)
{
	console.WriteString(pMsg, White, Black);
	while (1);
}

void Halt()
{
	while (1);
}