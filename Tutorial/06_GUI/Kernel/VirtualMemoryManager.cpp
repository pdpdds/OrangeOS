#include "VirtualMemoryManager.h"
#include "PhysicalMemoryManager.h"
#include "string.h"

VirtualMemoryManager::VirtualMemoryManager()
{
}


VirtualMemoryManager::~VirtualMemoryManager()
{
}

//가상 주소와 매핑된 실제 물리 주소를 얻어낸다.
void* VirtualMemoryManager::GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress)
{
	PDE* pagedir = directory->m_entries;
	if (pagedir[virtualAddress >> 22] == 0)
		return NULL;

	return (void*)((uint32_t*)(pagedir[virtualAddress >> 22] & ~0xfff))[virtualAddress << 10 >> 10 >> 12];
}

//페이지 테이블 엔트리 인덱스가 0이 아니면 이미 페이지 테이블이 존재한다는 의미
bool VirtualMemoryManager::CreatePageTable(PageDirectory* dir, uint32_t virt, uint32_t flags)
{
	PDE* pageDirectory = dir->m_entries;
	if (pageDirectory[virt >> 22] == 0) 
	{
		void* pBlock = PhysicalMemoryManager::GetInstance()->AllocBlock();
		if (pBlock == NULL)
			return false;

		pageDirectory[virt >> 22] = ((uint32_t)pBlock) | flags;
		memset(pBlock, 0, 4096);

		/* map page table into directory */
		MapPhysicalAddressToVirtualAddresss(dir, (uint32_t)pBlock, (uint32_t)pBlock, flags);
	}
	return true;
}

/**
* Map physical address to virtual
* \param dir Page directory
* \param virt Virtual address
* \param phys Physical address
* \param flags Page flags
*/
void VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(PageDirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags)
{
	PDE* pageDir = dir->m_entries;
	if (pageDir[virt >> 22] == 0)
		CreatePageTable(dir, virt, flags);
	((uint32_t*)(pageDir[virt >> 22] & ~0xfff))[virt << 10 >> 10 >> 12] = phys | flags;
}