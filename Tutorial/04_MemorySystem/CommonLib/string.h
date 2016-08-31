//Standard C String routines
#pragma once
#include <size_t.h>

char *strcpy(char *s1, const char *s2);
size_t strlen ( const char* str );
int strcmp (const char* str1, const char* str2);

void* memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, char val, size_t count);
unsigned short* memsetw(unsigned short *dest, unsigned short val, size_t count);

char* strchr (char * str, int character );