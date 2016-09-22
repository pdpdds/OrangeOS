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

		memset(pBlock, 0, PAGE_TABLE_SIZE);
		pageDirectory[virt >> 22] = ((uint32_t)pBlock) | flags;
		
		/* map page table into directory */
		MapPhysicalAddressToVirtualAddresss(dir, (uint32_t)pBlock, (uint32_t)pBlock, flags);
	}
	return true;
}

//PDE나 PTE의 플래그는 같은 값을 공유
//가상주소를 물리 주소에 매핑
void VirtualMemoryManager::MapPhysicalAddressToVirtualAddresss(PageDirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags)
{
	PDE* pageDir = dir->m_entries;
	if (pageDir[virt >> 22] == 0)
		CreatePageTable(dir, virt, flags);
	((uint32_t*)(pageDir[virt >> 22] & ~0xfff))[virt << 10 >> 10 >> 12] = phys | flags;
}

void VirtualMemoryManager::UnmapPageTable(PageDirectory* dir, uint32_t virt) 
{
	PDE* pageDir = dir->m_entries;
	if (pageDir[virt >> 22] != 0) {

		/* get mapped frame */
		void* frame = (void*)(pageDir[virt >> 22] & 0x7FFFF000);

		/* unmap frame */
		PhysicalMemoryManager::GetInstance()->FreeBlock(frame);		
		pageDir[virt >> 22] = 0;
	}
}


void VirtualMemoryManager::UnmapPhysicalAddress(PageDirectory* dir, uint32_t virt)
{
	PDE* pagedir = dir->m_entries;
	if (pagedir[virt >> 22] != 0)
		UnmapPageTable(dir, virt);
}

PageDirectory* VirtualMemoryManager::CreateAddressSpace()
{
	PageDirectory* dir = NULL;

	/* allocate page directory */
	dir = (PageDirectory*)PhysicalMemoryManager::GetInstance()->AllocBlock();
	if (!dir)
		return NULL;
	
	memset(dir, 0, sizeof(PageDirectory));

	return dir;
}

void VirtualMemoryManager::ClearPageDirectory(PageDirectory* dir)
{
	if (dir == NULL)
		return;
	
	memset(dir, 0, sizeof(PageDirectory));
}

PDE* VirtualMemoryManager::GetPDE(PageDirectory* dir, uint32_t addr)
{
	if (dir == NULL)
		return NULL;
		
	return &dir->m_entries[GetPageTableIndex(addr)];
}

uint32_t VirtualMemoryManager::GetPageTableIndex(uint32_t addr)
{
	return (addr >= DTABLE_ADDR_SPACE_SIZE) ? 0 : addr / PAGE_SIZE;
}

//살펴볼 것
uint32_t VirtualMemoryManager::GetPageTableEntryIndex(uint32_t addr) 
{
	
	return (addr >= PTABLE_ADDR_SPACE_SIZE) ? 0 : addr / PAGE_SIZE;
}

PTE* VirtualMemoryManager::GetPTE(PageTable* p, uint32_t addr) 
{
	if (p == NULL)
		return NULL;
	
	return &p->m_entries[GetPageTableEntryIndex(addr)];
}

void VirtualMemoryManager::ClearPageTable(PageTable* p) 
{
	if (p != NULL)
		memset(p, 0, sizeof(PageTable));
}

bool VirtualMemoryManager::AllocPage(PTE* e)
{	
	void* p = PhysicalMemoryManager::GetInstance()->AllocBlock();
	
	if (p == NULL)
		return false;
	
	PageTableEntry::SetFrame(e, (uint32_t)p);
	PageTableEntry::AddAttribute(e, I86_PTE_PRESENT);

	return true;
}

void VirtualMemoryManager::FreePage(PTE* e) 
{

	void* p = (void*)PageTableEntry::GetFrame(*e);
	if (p)
		PhysicalMemoryManager::GetInstance()->FreeBlock(p);		

	PageTableEntry::DelAttribute(e, I86_PTE_PRESENT);
}