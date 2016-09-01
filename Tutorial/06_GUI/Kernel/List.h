#pragma once

#include "windef.h"

#define KEYSIZE 30
            
struct ListNode
{
    void * ptrData;
    char szKey[KEYSIZE];
    struct ListNode * Next;
};

typedef struct ListNode LISTNODE;
typedef  LISTNODE * LPLISTNODE;

int List_Count(LISTNODE *List);
int List_Add(LISTNODE ** Root, LPCTSTR szKey, void * ptrData);
int List_Delete(LISTNODE ** Root, LPCTSTR szKey, int Index);
void * List_GetData(LISTNODE * List, LPCTSTR szKey, int Index);
void List_Dump(LISTNODE * List);
    
#define FOREACH(tmpNode, ListRoot) \
    for( tmpNode=ListRoot; tmpNode != NULL ; tmpNode = tmpNode->Next ) 
    


