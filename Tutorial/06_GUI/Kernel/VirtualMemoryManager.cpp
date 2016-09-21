#include "VirtualMemoryManager.h"



VirtualMemoryManager::VirtualMemoryManager()
{
}


VirtualMemoryManager::~VirtualMemoryManager()
{
}

/**
* Get physical address from virtual
* \param dir Page directory
* \param virt Virtual address
* \ret Physical address
*/
void* VirtualMemoryManager::GetPhysicalAddressFromVirtualAddress(pdirectory* dir, uint32_t virt)
{
	pd_entry* pagedir = dir->m_entries;
	if (pagedir[virt >> 22] == 0)
		return 0;
	return (void*)((uint32_t*)(pagedir[virt >> 22] & ~0xfff))[virt << 10 >> 10 >> 12];
}