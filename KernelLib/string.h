#pragma once
#include "windef.h"
#include "Macro.h"

/*---------------------------------------------------------------------------*/
/* function prototpes                                                        */
/*---------------------------------------------------------------------------*/

void ntostr(unsigned int i, char *s);		//Converts a number to a string (needs replacement)

void *memccpy(void *dest, const void *src, int c, unsigned int count);
void *memchr(const void *buf, int c, size_t count);
int memcmp(const void *buf1, const void *buf2, size_t count);
int memicmp(const void *buf1, const void *buf2, unsigned int count);
void *memcpy(void *dest, const void *src, size_t count);
void *memmove(void *dest, const void *src, size_t count);
void *memset(void *dest, int c, size_t count);

int strnicmp(const char *string1, const char *string2, size_t count);
char *strnchr(const char *string, int c,size_t count);


char *strcat(char *string1, const char *string2);
char *strchr(const char *string, int c);
int strcmp(const char *string1, const char *string2);
int stricmp(const char *string1, const char *string2);
char *strcpy(char *string1, const char *string2);
size_t strcspn(const char *string1, const char *string2);
char *strdup(const char *string);
char *strerror(int errnum);
size_t strlen(const char *string);
char *strlwr(char *string);
char *strncat(char *string1, const char *string2, size_t count);
int strncmp(const char *string1, const char *string2, size_t count);

char *strncpy(char *string1, const char *string2, size_t count);
char *strnset(char *string, int c, size_t count);
char *strpbrk(const char *string1, const char *string2);
char *strrchr(const char *string, int c);
char *strrev(char *string);
char *strset(char *string, int c);
size_t strspn(const char *string1, const char *string2);
char *strstr(const char *string1, const char *string2);
char *strtok(char *string1, const char *string2);
char *strupr(char *string);

void MergePath(char *path, const char *drive, const char *dir, const char *name, const char *ext);
int SplitPath(const char *path, char *drive, char *dir, char *name, char *ext);
BYTE PatternSearch(const char * szText, const char * szPattern);

/*
static char* MaxPtr(char * p1, char * p2)
{
    if (p1 > p2)
        return p1;
    else
        return p2;
}

static DWORD ConvertWordsToDoubleWord(WORD wHigh, WORD wLow)
{
    DWORD dwResult;
    dwResult = wHigh;
    dwResult <<= 16;
    dwResult |= wLow;
    return dwResult;
}
*/


