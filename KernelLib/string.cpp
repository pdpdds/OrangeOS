#include "string.h"

/*---------------------------------------------------------------------------*/
/* strlen() - get the length of a string                                     */
/*---------------------------------------------------------------------------*/
size_t strlen(const char *string)
{
  size_t slen = 0;
  while(*string)
  {
    slen++;
    string++;
  }
  return(slen);
}

/*---------------------------------------------------------------------------*/
/* memcmp() - compare characters in two buffers                              */
/*---------------------------------------------------------------------------*/
int memcmp(const void *buf1, const void *buf2, size_t count)
{
  char *p1, *p2;
  int result = 0;
  p1 = (char *)buf1;
  p2 = (char *)buf2;
  while(count)
  {
    if(*p1 != *p2)
    {
      result = 1;
      if(*p1 < *p2)
      {
        result = -1;
      }
      break;
    }
    p1++;
    p2++;
    count--;
  }
  return(result);
}



/*---------------------------------------------------------------------------*/
/* memcpy() - copy characters from a buffer                                  */
/*---------------------------------------------------------------------------*/
void *memcpy(void *dest, const void *src, size_t count)
{
  char *p1, *p2;
  p1 = (char *)src;
  p2 = (char *)dest;
  while(count)
  {
    *p2 = *p1;
    p1++;
    p2++;
    count--;
  }
  return(dest);
}

/*---------------------------------------------------------------------------*/
/* memset() - set a buffer to a specified character                          */
/*---------------------------------------------------------------------------*/

void *memset(void *dest, int c, size_t count)
{
  char *p = (char *)dest;
  while(count)
  {
    *p = (char)c;
    p++;
    count--;
  }
  return(dest);
}


//Converts a number to a base 10 digit (requires replacement)
void ntostr(unsigned int i, char *s)
{
   unsigned char backstr[21], j=0, l=0, m=0;

   do							// Convert string one digit at a time
   {
      backstr[j++] = (i % 10) + '0';	//Put a digit in backstr
      i /= 10;							//get next digit
   }
   while(i);

   backstr[j] = '\0';			//End of the string

   for(l=j-1; m<j; l--)			//Backstr is backwards
   {							// Now we flip it
      s[m++] = backstr[l];
   }

   s[j] = '\0';
}

int strcmp(const char *string1, const char *string2)
{
  int result = 0;
  while((*string1 == *string2) && (!result))
  {
    if(!(*string1) || !(*string2))
    {
      result = 1;
    } else {
      string1++;
      string2++;
    }
  }
  result = 0;
  if(*string1 != *string2)
  {
    result = 1;
    if(*string1 < *string2)
    {
      result = -1;
    }
  }
  return(result);
}

/*---------------------------------------------------------------------------*/
/* strcpy() - copy a string to another                                       */
/*---------------------------------------------------------------------------*/

char *strcpy(char *string1, const char *string2)
{
  char *p = string1;
  while(*string2)
  {
    *p = *string2;
    p++;
    string2++;
  }
  *p = '\0';
  return(string1);
}

/*---------------------------------------------------------------------------*/
/* memmove() - move one buffer to another                                    */
/*---------------------------------------------------------------------------*/

void *memmove(void *dest, const void *src, size_t count)
{
  char *p1, *p2;
  if(src != dest)
  {
    p1 = (char *)src;
    p2 = (char *)dest;
    if(dest > src)
    {
      p1 += (count - 1);
      p2 += (count - 1);
      while(count)
      {
        *p2 = *p1;
        p1--;
        p2--;
        count--;
      }
    } else {
      while(count)
      {
        *p2 = *p1;
        p1++;
        p2++;
        count--;
      }
    }
  }
  return(dest);
}

/*---------------------------------------------------------------------------*/
/* strupr() - convert a string to upper case                                 */
/*---------------------------------------------------------------------------*/

char *strupr(char *string)
{
  char *p = string;
  while(*p)
  {
    *p = (char)toupper(*p);
    p++;
  }
  return(string);
}

/*---------------------------------------------------------------------------*/
/* strstr() - find a substring                                               */
/*---------------------------------------------------------------------------*/
char *strstr(const char *string1, const char *string2)
{
  char *p1, *p2, *p3;
  int found = 0;
  while(*string1 && (!found))
  {
    p1 = (char *)string1;
    p2 = (char *)string2;
    p3 = 0;
    found = 1;
    while(*p2)
    {
      if(*p1 != *p2)
      {
        found = 0;
        break;
      } else {
        p1++;
        p2++;
      }
    }
    if(!found)
    {
     string1++;
    } else {
      p3 = (char *)string1;
    }
  }
  return(p3);
}

/*---------------------------------------------------------------------------*/
/* strspn() - find the first substring                                       */
/*---------------------------------------------------------------------------*/
size_t strspn(const char *string1, const char *string2)
{
  char *p;
  int found = 1;
  size_t slen = 0;
  while(*string1 && (found))
  {
    found = 0;
    p = (char *)string2;
    while(*p)
    {
      if(*string1 == *p)
      {
        slen++;
        found = 1;
        break;
      }
      p++;
    }
    if(found)
    {
      string1++;
    }
  }
  return(slen);
}

/*---------------------------------------------------------------------------*/
/* strset() - set the contents of a string to a character                    */
/*---------------------------------------------------------------------------*/

char *strset(char *string, int c)
{
  char *p = string;
  while(*p)
  {
    *p = (char)c;
    p++;
  }
  return(string);
}

/*---------------------------------------------------------------------------*/
/* strrev() - reverse characters in a string                                 */
/*---------------------------------------------------------------------------*/
char *strrev(char *string)
{
  char temp;
  size_t slen;
  char *p1, *p2;
  slen = strlen(string);
  if(slen > 1)
  {
    p1 = string;
    p2 = string + (slen - 1);
    while (p1 < p2)
    {
      temp = *p1;
      *p1 = *p2;
      *p2 = temp;
      p1++;
      p2--;
    }
  }
  return(string);
}

/*---------------------------------------------------------------------------*/
/* strrchr() - find the last occurence of a character in a string            */
/*---------------------------------------------------------------------------*/
char *strrchr(const char *string, int c)
{
  char *p = NULL;
  while(*string)
  {
    if((int)(*string) == c)
    {
      p = (char *)string;
    }
    string++;
  }
  return(p);
}

/*---------------------------------------------------------------------------*/
/* strpbrk() - point to any one of a set of characters within a string       */
/*---------------------------------------------------------------------------*/
char *strpbrk(const char *string1, const char *string2)
{
  char *p;
  char *p1 = NULL;
  int found = 0;
  while(*string1 && (!found))
  {
    p = (char *)string2;
    while(*p)
    {
      if(*string1 == *p)
      {
        p1 = (char *)string1;
        found = 1;
        break;
      }
      p++;
    }
    if(!found)
    {
      string1++;
    }
  }
  return(p1);
}

/*---------------------------------------------------------------------------*/
/* strnset() - set characters in a string                                    */
/*---------------------------------------------------------------------------*/
char *strnset(char *string, int c, size_t count)
{
  char *p = string;
  while(*p && count)
  {
    *p = (char)c;
    count--;
    p++;
  }
  return(string);
}

//////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////
int strnicmp(const char *string1, const char *string2, size_t count)
{
  int result = 0;
  while((toupper(*string1) == toupper(*string2)) && (count > 1))
  {
    count--;
    if(!(*string1) || !(*string2))
    {
      count = 0;
    } else {
      string1++;
      string2++;
    }
  }
  result = 0;
  if(toupper(*string1) > toupper(*string2))
  {
    result = 1;
  } else {
    if(toupper(*string1) < toupper(*string2))
    {
      result = -1;
    }
  }
  return(result);
}

/*---------------------------------------------------------------------------*/
/* strncpy() - copy characters from one string to another                    */
/*---------------------------------------------------------------------------*/
char *strncpy(char *string1, const char *string2, size_t count)
{
  char *p = string1;
  while(count)
  {
    *p = *string2;
    if(*string2)
    {
     string2++;
    }
    p++;
    count--;
  }
  return(string1);
}

/*---------------------------------------------------------------------------*/
/* strncmp() - compare characters of two strings                             */
/*---------------------------------------------------------------------------*/
int strncmp(const char *string1, const char *string2, size_t count)
{
  int result = 0;
  while((*string1 == *string2) && (count > 1))
  {
    count--;
    if(!(*string1) || !(*string2))
    {
      count = 0;
    } else {
      string1++;
      string2++;
    }
  }
  result = 0;
  if(*string1 > *string2)
  {
    result = 1;
  } else {
    if(*string1 < *string2)
    {
      result = -1;
    }
  }
  return(result);
}

//////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////
char *strnchr(const char *string, int c,size_t count)
{
  char *p = NULL;
  while(*string && count > 0)
  {
    if((int)(*string) == c)
    {
      p = (char *)string;
      break;
    }
    string++;
    count--;
  }
  return(p);
}

/*---------------------------------------------------------------------------*/
/* strncat() - concatenate characters to a string                            */
/*---------------------------------------------------------------------------*/
char *strncat(char *string1, const char *string2, size_t count)
{
  char *p;
  p = string1 + strlen(string1);
  while(*string2 && count)
  {
    *p = *string2;
    count--;
    string2++;
    p++;
  }
  *p = '\0';
  return(string1);
}

/*---------------------------------------------------------------------------*/
/* strlwr() - convert a string to lower case                                 */
/*---------------------------------------------------------------------------*/
char *strlwr(char *string)
{
  char *p = string;
  while(*p)
  {
    *p = (char)tolower(*p);
    p++;
  }
  return(string);
}

/*---------------------------------------------------------------------------*/
/* stricmp() - compare strings (case insensitive)                            */
/*---------------------------------------------------------------------------*/
int stricmp(const char *string1, const char *string2)
{
  int result = 0;
  while((toupper(*string1) == toupper(*string2)) && (!result))
  {
    if(!(*string1) || !(*string2))
    {
      result = 1;
    } else {
      string1++;
      string2++;
    }
  }
  result = 0;
  if(*string1 != *string2)
  {
    result = 1;
    if(toupper(*string1) < toupper(*string2))
    {
      result = -1;
    }
  }
  return(result);
}

/*---------------------------------------------------------------------------*/
/* strdup() - create a duplicate string                                      */
/*---------------------------------------------------------------------------*/
/*char *strdup(const char *string)
{
  char *p;
  size_t slen;
  slen = strlen(string);
  p = malloc(slen + 1);
  strcpy(p, string);
  return(p);
}*/

/*---------------------------------------------------------------------------*/
/* strcspn() - find any one of a set of characters within a string           */
/*---------------------------------------------------------------------------*/
size_t strcspn(const char *string1, const char *string2)
{
  char *p;
  size_t pos = 0;
  short found = 0;
  while(*string1 && (!found))
  {
    p = (char *)string2;
    while(*p)
    {
      if(*string1 == *p)
      {
        found = 1;
        break;
      }
      p++;
    }
    if(!found)
    {
      pos++;
      string1++;
    }
  }
  return(pos);
}

/*---------------------------------------------------------------------------*/
/* strchr() - find a character in a string                                   */
/*---------------------------------------------------------------------------*/
char *strchr(const char *string, int c)
{
  char *p = NULL;
  while(*string)
  {
    if((int)(*string) == c)
    {
      p = (char *)string;
      break;
    }
    string++;
  }
  return(p);
}

char *strcat(char *string1, const char *string2)
{
  char *p= string1 + strlen(string1);
  while(*string2)
  {
    *p = *string2;
    string2++;
    p++;
  }
  *p = '\0';
  return(string1);
}

/*---------------------------------------------------------------------------*/
/* memicmp() - compare characters in two buffers (case insensitive)          */
/*---------------------------------------------------------------------------*/
int memicmp(const void *buf1, const void *buf2, unsigned int count)
{
  char *p1, *p2;
  int result = 0;
  p1 = (char *)buf1;
  p2 = (char *)buf2;
  while(count)
  {
    if(toupper(*p1) != toupper(*p2))
    {
      result = 1;
      if(toupper(*p1) < toupper(*p2))
      {
        result = -1;
      }
      break;
    }
    p1++;
    p2++;
    count--;
  }
  return(result);
}

/*---------------------------------------------------------------------------*/
/* memchr() - find a character in a buffer                                   */
/*---------------------------------------------------------------------------*/
void *memchr(const void *buf, int c, size_t count)
{
  char *p1 = NULL;
  char *p2;
  p2 = (char *)buf;
  while(count)
  {
    if((int)(*p2) == c)
    {
      p1 = p2;
      break;
    }
    p2++;
    count--;
  }
  return(p1);
}

/*---------------------------------------------------------------------------*/
/* memccpy() - copy characters from a buffer                                 */
/*---------------------------------------------------------------------------*/
void *memccpy(void *dest, const void *src, int c, unsigned int count)
{
  char *p1, *p2;
  p1 = (char *)src;
  p2 = (char *)dest;
  while(count)
  {
    *p2 = *p1;
    if(*p2 == (char)c)
    {
      dest = (p2 + 1);
      break;
    }
    p2++;
    p1++;
    count--;
  }
  return(dest);
}


char * ExtractFirstToken(char * lpBuffer, char * szSourceString, char cTokenSeparator)
{
    char * szToken = 0;
    int i;

    lpBuffer[0] = 0;
    for(i=0; szSourceString[i]; i++)
		if ( szSourceString[i] == cTokenSeparator )
		{
			szToken = &szSourceString[i];
			break;
		}
    if ( !szToken )
        return 0;

	strncpy( lpBuffer, szSourceString, szToken-szSourceString);
    lpBuffer[ szToken - szSourceString ]=0;

    return szToken;
}
BYTE PatternSearch(const char * szText, const char * szPattern)
{
    int  i, slraw;
    #ifdef _DEBUG_
        dbgprintf("\n UTIL :: PattenSearch [%s], [%s]", szText, szPattern);
    #endif

    if ((*szPattern == '\0') && (*szText == '\0'))    /*  if it is end of both  */
        return 1;                                     /*  strings,then match    */
    if (*szPattern == '\0')                           /*  if it is end of only  */
        return 0;                                 /*  szPattern tehn mismatch     */
    if (*szPattern == '*')                            /*  if pattern is a '*'    */
    {
        if (*(szPattern+1) == '\0')                   /*  if it is end of szPattern */
            return 1;                             /*    then match          */
        for(i=0,slraw=strlen(szText);i<=slraw;i++)    /*    else hunt for match*/
            if ((*(szText+i) == *(szPattern+1)) ||    /*         or wild card   */
                                (*(szPattern+1) == '?'))
                if (PatternSearch(szText+i+1,szPattern+2) == 1)    /*      if found,match    */
                    return( 1 ) ;                       /*        rest of szPattern     */
    }
    else
    {
        if ( *szText == '\0' )                          /*  if end of szText then    */
            return 0;                                   /*     mismatch           */
        if ( (*szPattern == '?') || (*szPattern == *szText) )  /*  if chars match then   */
            if ( PatternSearch(szText+1,szPattern+1) == 1)     /*  try & match rest of it*/
               return 1;
    }
    #ifdef _DEBUG_
        dbgprintf("\n UTIL :: PattenSearch No match");
    #endif
    return 0;                                                  /*  no match found        */
}

int SplitPath(const char *path, char *drive, char *dir, char *name, char *ext)
{
    int flags = 0, len;
    const char *pp, *pe;
    if (drive)
        *drive = '\0';
    if (dir)
        *dir = '\0';
    if (name)
        *name = '\0';
    if (ext)
        *ext = '\0';

    pp = path;
    if ((isalpha(*pp) || strchr("[\\]^_`", *pp)) && (pp[1] == ':'))
    {
        if (drive)
        {
            strncpy(drive, pp, 2);
            drive[2] = '\0';
        }
        pp += 2;
    }
	//20150920
    //pe = MaxPtr( (char *) strrchr(pp, '\\'),(char *) strrchr(pp, '/'));
	pe = ((char *)strrchr(pp, '\\'), (char *)strrchr(pp, '/'));
    if (pe)
    {
        pe++;
        len = pe - pp;
        if (dir)
        {
            strncpy(dir, pp, len);
            dir[len] = '\0';
        }
        pp = pe;
    }
    else
        pe = pp;
    if ( *pp == NULL ) //sam
        return 0;
    /* Special case: "c:/path/." or "c:/path/.."
    These mean FILENAME, not EXTENSION.  */
    while (*pp == '.')
        ++pp;
    if (pp > pe)
    {
        if (name)
        {
            len = pp - pe;
            strncpy(name, pe, len);
            name[len] = '\0';
          /* advance name over '.'s so they don't get scragged later on when the
           * rest of the name (if any) is copied (for files like .emacs). - WJC
           */
            name+=len;
        }
    }

    pe = strrchr(pp, '.');
    if (pe)
    {
        if (ext)
            strcpy(ext, pe);
    }
    else
        pe =  pp + strlen(pp);

    if (pp != pe)
    {

        if ( pe > pp )
            len = pe - pp;
        else
            len = 0;
        if (name)
        {
            strncpy(name, pp, len);
            name[len] = '\0';
        }
    }
    return flags;
}
void MergePath(char *path, const char *drive, const char *dir, const char *name, const char *ext)
{
    *path = '\0';
    if (drive && *drive)
    {
        path[0] = drive[0];
        path[1] = ':';
        path[2] = 0;
    }
    if (dir && *dir)
    {
        char last_dir_char = dir[strlen(dir) - 1];
        strcat(path, dir);
        if (last_dir_char != '/' && last_dir_char != '\\')
                strcat(path, strchr(dir, '\\') ? "\\" : "/");
    }
    if (name)
        strcat(path, name);
    if (ext && *ext)
    {
        if (*ext != '.')
            strcat(path, ".");
        strcat(path, ext);
    }
}


