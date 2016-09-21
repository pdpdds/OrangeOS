#include "VirtualMemoryManager.h"



VirtualMemoryManager::VirtualMemoryManager()
{
}


VirtualMemoryManager::~VirtualMemoryManager()
{
}

//���� �ּҿ� ���ε� ���� ���� �ּҸ� ����.
void* VirtualMemoryManager::GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress)
{
	pd_entry* pagedir = dir->m_entries;
	if (pagedir[virt >> 22] == 0)
		return 0;
	return (void*)((uint32_t*)(pagedir[virt >> 22] & ~0xfff))[virt << 10 >> 10 >> 12];
}