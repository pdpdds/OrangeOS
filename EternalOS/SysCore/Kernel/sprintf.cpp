#include "sprintf.h"
#include <stdarg.h>
#include <va_list.h>
#include <stdio.h>

int __cdecl sprintf(char *s, const char *format, ...)
{
	va_list arg;
	int ret;

	va_start(arg, format);
	ret = vsprintf(s, format, arg);
	va_end(arg);

	return ret;
}