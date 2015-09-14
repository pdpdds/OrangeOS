/*
 * printf.h
 *
 *  Created on: 2012. 2. 26.
 *      Author: darkx
 */

#ifndef PRINTF_H_
#define PRINTF_H_
#include <StdArg.h>

void vsprintf(char *A_szString, const char *A_szFormat, va_list A_pAp);
int print_decimal(char *A_szBuffer, int a, char flag);
int print_hex(char *buf, int a, char flag);
int __cdecl printf(const char *format, ...);

void itoa_s(int i, unsigned int base, char* buf);
void itoa(unsigned i, unsigned base, char* buf);


#endif /* PRINTF_H_ */
