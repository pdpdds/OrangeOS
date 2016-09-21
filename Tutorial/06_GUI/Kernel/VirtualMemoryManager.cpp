#include "VirtualMemoryManager.h"



VirtualMemoryManager::VirtualMemoryManager()
{
}


VirtualMemoryManager::~VirtualMemoryManager()
{
}

//가상 주소와 매핑된 실제 물리 주소를 얻어낸다.
void* VirtualMemoryManager::GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress)
{
	pd_entry* pagedir = dir->m_entries;
	if (pagedir[virt >> 22] == 0)
		return 0;
	return (void*)((uint32_t*)(pagedir[virt >> 22] & ~0xfff))[virt << 10 >> 10 >> 12];
}