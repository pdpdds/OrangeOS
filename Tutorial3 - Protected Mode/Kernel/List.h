#ifndef __LIST_H
#define __LIST_H
/* 
        Ace OS - Source Code
    Linked List Implementation - Created by Sam ( 28-May-2003)
				samuelhard@yahoo.com
*/
#include "windef.h"

#ifdef __cplusplus
	extern "C" {
#endif
            
#define KEYSIZE 30
            
struct ListNode
{
    void * ptrData;
    char szKey[KEYSIZE];
    struct ListNode * Next;
};

typedef struct ListNode LISTNODE;
typedef  LISTNODE * LPLISTNODE;

int List_Count(struct ListNode *List);
int List_Add(struct ListNode ** Root, LPCTSTR szKey, void * ptrData);
int List_Delete(struct ListNode ** Root, LPCTSTR szKey, int Index);
void * List_GetData(struct ListNode * List, LPCTSTR szKey,int Index);
void List_Dump(struct ListNode * List);
    
#define FOREACH(tmpNode, ListRoot) \
    for( tmpNode=ListRoot; tmpNode != NULL ; tmpNode = tmpNode->Next ) 
    
#ifdef __cplusplus
	}
#endif
#endif


