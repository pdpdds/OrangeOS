#pragma once

int __cdecl sprintf(char *s, const char *format, ...);
void __cdecl itoa(unsigned i, unsigned base, char* buf);
void __cdecl itoa_s(int i, unsigned int base, char* buf);
void * __cdecl memset(char *dest, int c, size_t count);
char * __cdecl strncpy(char *string1, const char *string2, size_t count);
int strcmp(const char *string1, const char *string2);
size_t strlen(const char *string);