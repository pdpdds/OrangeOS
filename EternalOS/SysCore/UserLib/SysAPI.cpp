#include "sysapi.h"

uint32_t GetTickCount()
{
	int address = 0;
	__asm
	{
		mov eax, 5
			int 0x80
			mov address, eax
	}
	return address;
}
typedef unsigned int   u32int;

void free(void *p)
{
	__asm {

		mov ebx, p
			mov eax, 3
			int 0x80
	}
}

u32int malloc(u32int sz)
{
	int address = 0;
	__asm {
		mov ebx, sz
			mov eax, 2
			int 0x80
			mov address, eax
	}

	return address;
}

void *operator new(size_t size)
{
	return (void *)malloc(size);
}

void *operator new[](size_t size)
{
	return (void *)malloc(size);
}

void operator delete(void *p)
{
	free(p);
}

int __cdecl _purecall()
{
	// Do nothing or print an error message.
	return 0;
}

void operator delete[](void *p)
{
	free(p);
}

char *strcpy(char *s1, const char *s2)
{
	char *s1_p = s1;
	while (*s1++ = *s2++);
	return s1_p;
}