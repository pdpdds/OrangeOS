#pragma once

typedef int uint32_t;
typedef unsigned int   u32int;

uint32_t GetTickCount();
void free(void *p);
u32int malloc(u32int sz);
void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
void operator delete(void *p, unsigned int);
int __cdecl _purecall();
void operator delete[](void *p);
