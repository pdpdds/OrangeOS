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

	if (base == 16)
	{
		itoa(i, base, buf);
	}
	else  if (i < 0) {
		*buf++ = '-';
		i *= -1;
	}
	itoa(i, base, buf);
}

//! copies string s2 to s1
char *strcpy(char *s1, const char *s2)
{
	char *s1_p = s1;
	while (*s1++ = *s2++);
	return s1_p;
}

//! returns length of string
/*size_t strlen(const char* str) {

	size_t	len = 0;
	while (str[len++]);
	return len;
}*/

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

char *strncpy(char *string1, const char *string2, size_t count)
{
	char *p = string1;
	while (count)
	{
		*p = *string2;
		if (*string2)
		{
			string2++;
		}
		p++;
		count--;
	}
	return(string1);
}

int strcmp(const char *string1, const char *string2)
{
	int result = 0;
	while ((*string1 == *string2) && (!result))
	{
		if (!(*string1) || !(*string2))
		{
			result = 1;
		}
		else {
			string1++;
			string2++;
		}
	}
	result = 0;
	if (*string1 != *string2)
	{
		result = 1;
		if (*string1 < *string2)
		{
			result = -1;
		}
	}
	return(result);
}

/*---------------------------------------------------------------------------*/
/* strlen() - get the length of a string                                     */
/*---------------------------------------------------------------------------*/
size_t strlen(const char *string)
{
	size_t slen = 0;
	while (*string)
	{
		slen++;
		string++;
	}
	return(slen);
}