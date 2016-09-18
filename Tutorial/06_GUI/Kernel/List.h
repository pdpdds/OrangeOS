#pragma once
#include "windef.h"
            
struct ListNode
{
    void* ptrData;    
    struct ListNode* Next;
};

namespace Orange
{
	class LinkedList
	{
	public:
		LinkedList();

		int Count();
		BOOL Add(void* ptrData);
		BOOL Delete(void* ptrData);
		void* Get(int index);

	private:
		ListNode* m_pNode;

	};
};

//#define FOREACH(tmpNode, ListRoot) \
    //for( tmpNode=ListRoot; tmpNode != NULL ; tmpNode = tmpNode->Next ) 
    


