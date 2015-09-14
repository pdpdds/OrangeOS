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

void *memset(char *dest, int c, size_t count)
{
	char *p = dest;
	while (count)
	{
		*p = (char)c;
		p++;
		count--;
	}
	return(dest);
}

Console orangeConsole;
extern "C" int _outp(unsigned short, int);
extern "C" int _outl(unsigned short, int);
#define OutPortByte(prt, val) _outp(prt, val)
#define OutPortWord(prt, val) _outpw(prt, val)
typedef unsigned char UCHAR;
#define TIMEOUT_PER_SECOND		50				/* system timer (irq0) */

static bool HalpInitPIC(void)
{
	/* master 8259 */
	OutPortByte(0x20, 0x11); /* edge trig, cascade mode, ICW4 */
	OutPortByte(0x21, 0x20); /* start at INT vector 20h */
	OutPortByte(0x21, 0x04); /* #IR2 pin has a slave */
	OutPortByte(0x21, 0x01); /* not sp, nonbuffered mode, normal EOI, for x86 */
	OutPortByte(0x21, 0x00);

	/* slave 8259 */
	OutPortByte(0xa0, 0x11); /* edge trig, cascade mode, ICW4 */
	OutPortByte(0xa1, 0x28); /* start at INT vector 28h */
	OutPortByte(0xa1, 0x02); /* slave 8259 has been linked to master's #IR2 pin */
	OutPortByte(0xa1, 0x01); /* not sp, nonbuffered mode, normal EOI, for x86 */
	OutPortByte(0xa1, 0x00);

	return TRUE;
}

static bool HalpInitSysTimer(BYTE timeoutPerSecond)
{
	WORD timeout = (WORD)(1193180 / timeoutPerSecond);

	OutPortByte(0x43, 0x34);
	OutPortByte(0x40, (UCHAR)(timeout & 0xff));
	OutPortByte(0x40, (UCHAR)(timeout >> 8));

	return TRUE;
}

void DbgError(char* pMsg)
{
	orangeConsole.WriteString(pMsg, White, Black);
	while (1);
}

void _cdecl main () {
	
	orangeConsole.Clear();
	char* szWelcome = "Welcome to Orange OS!!";

	if (!HalpInitPIC()) {		
		DbgError("HalpInitPIC() returned an error");
	}
	if (!HalpInitSysTimer(TIMEOUT_PER_SECOND)) {
		DbgError("HalpInitSysTimer() returned an error");
	}

	
 
	
	char szBuf[256];
	//memset(szBuf, 0, 256);
	strcpy(szBuf, szWelcome);	
	
	orangeConsole.WriteString(szBuf, White, Black);
	orangeConsole.WriteString("This is a C++ OS.", White, Black);

	//while (1)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		//sysTime.wYear = 0;
		char szBuf[256];
		memset(szBuf, 0, 256);
		sprintf(szBuf, "LocalTime : %d/%d %d:%d", (int)sysTime.wMonth, (int)sysTime.wDay, (int)sysTime.wHour, (int)sysTime.wMinute);
		
		orangeConsole.WriteString(szBuf, White, Black);
	}
	while (1);
}

//! copies string s2 to s1
char *strcpy(char *s1, const char *s2)
{
	char *s1_p = s1;
	while (*s1++ = *s2++);
	return s1_p;
}

//! returns length of string
size_t strlen(const char* str) {

	size_t	len = 0;
	while (str[len++]);
	return len;
}

char tbuf[32];
char bchars[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
void itoa(unsigned i, unsigned base, char* buf) {
	int pos = 0;
	int opos = 0;
	int top = 0;

	if (i == 0 || base > 16) {
		buf[0] = '0';		
		buf[1] = '\0';
		return;
	}

	while (i != 0) {
		tbuf[pos] = bchars[i % base];
		pos++;
		i /= base;
	}
	top = pos--;
	for (opos = 0; opos<top; pos--, opos++) {
		buf[opos] = tbuf[pos];
	}
	buf[opos] = 0;
}

void itoa_s(int i, unsigned int base, char* buf) {
	if (base > 16) return;
	if (i < 0) {
		*buf++ = '-';
		i *= -1;
	}
	itoa(i, base, buf);
}