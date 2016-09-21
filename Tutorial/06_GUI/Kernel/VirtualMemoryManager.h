#pragma once
#include "windef.h"
#include "stdint.h"
#include "PageDirectoryEntry.h"
#include "PageTableEntry.h"

using namespace PageTableEntry;
using namespace PageDirectoryEntry;

// I86 아키텍쳐에서는 페이지테이블이나 페이지 디렉토리가 각각 1024개의 엔트리를 가진다
// 32비트에서 엔트리의 크기는 4바이트다. 
// 크기(4 * 1024 = 4K)
// 따라서 한 프로세스는 가상주소 4기가를 전부 표현하기 위해 약 4메가바이트의
// 메모리 공간을 필요로 한다(4K(PDE) + 1024 * 4K(PTE))
// 하나의 페이지 테이블은 4MB를 표현할 수 있고 페이지 디렉토리에 1024개의 페이지 테이블이
// 존재하므로 4MB * 1024 = 4GB, 즉 4GB 바이트 전체를 표현할 수 있다.

#define PAGES_PER_TABLE		1024
#define PAGES_PER_DIRECTORY	1024

typedef struct tag_PageTable 
{
	PTE m_entries[PAGES_PER_TABLE];
}PageTable;

typedef struct tag_PageDirectory 
{
	PDE m_entries[PAGES_PER_DIRECTORY];
}PageDirectory;

class VirtualMemoryManager
{
public:
	VirtualMemoryManager();
	virtual ~VirtualMemoryManager();

	void* GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress);


	//! initialize the memory manager
	void vmmngr_initialize();

	//! allocates a page in physical memory
	bool vmmngr_alloc_page(PTE*);

	//! frees a page in physical memory
	void vmmngr_free_page(PTE* e);

	//! switch to a new page directory
	bool vmmngr_switch_pdirectory(PageDirectory*);

	//! get current page directory
	PageDirectory* vmmngr_get_directory();

	//! flushes a cached translation lookaside buffer (TLB) entry
	void vmmngr_flush_tlb_entry(uint32_t addr);

	//! clears a page table
	void vmmngr_ptable_clear(PageTable* p);

	//! convert virtual address to page table index
	uint32_t vmmngr_ptable_virt_to_index(uint32_t addr);

	//! get page entry from page table
	PTE* vmmngr_ptable_lookup_entry(PageTable* p, uint32_t addr);

	//! convert virtual address to page directory index
	uint32_t vmmngr_pdirectory_virt_to_index(uint32_t addr);

	//! clears a page directory table
	void vmmngr_pdirectory_clear(PageDirectory* dir);

	//! get directory entry from directory table
	PDE* vmmngr_pdirectory_lookup_entry(PageDirectory* p, uint32_t addr);


	bool CreatePageTable(PageDirectory* dir, uint32_t virt, uint32_t flags);
	void MapPhysicalAddressToVirtualAddresss(PageDirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags);
	void        vmmngr_unmapPageTable(PageDirectory* dir, uint32_t virt);
	void        vmmngr_unmapPhysicalAddress(PageDirectory* dir, uint32_t virt);
	PageDirectory* vmmngr_createAddressSpace();
	void*       vmmngr_getPhysicalAddress(PageDirectory* dir, uint32_t virt);


	static VirtualMemoryManager* GetInstance()
	{
		return &m_virtualMemoryManager;
	}

private:
	static VirtualMemoryManager m_virtualMemoryManager;
};

