#pragma once
#include "windef.h"
#include "stdint.h"
#include "PageDirectoryEntry.h"
#include "PageTableEntry.h"


#define KERNEL_VIRTUAL_BASE_ADDRESS 0xC0000000
#define KERNEL_VIRTUAL_HEAP_ADDRESS 0xD0000000
#define KERNEL_VIRTUAL_STACK_ADDRESS 0xCF000000
#define KERNEL_PHYSICAL_BASE_ADDRESS 0x100000

using namespace PageTableEntry;
using namespace PageDirectoryEntry;

// I86 ��Ű���Ŀ����� ���������̺��̳� ������ ���丮�� ���� 1024���� ��Ʈ���� ������
// 32��Ʈ���� ��Ʈ���� ũ��� 4����Ʈ��. 
// ũ��(4 * 1024 = 4K)
// ���� �� ���μ����� �����ּ� 4�Ⱑ�� ���� ǥ���ϱ� ���� �� 4�ް�����Ʈ��
// �޸� ������ �ʿ�� �Ѵ�(4K(PDE) + 1024 * 4K(PTE))
// �ϳ��� ������ ���̺��� 4MB�� ǥ���� �� �ְ� ������ ���丮�� 1024���� ������ ���̺���
// �����ϹǷ� 4MB * 1024 = 4GB, �� 4GB ����Ʈ ��ü�� ǥ���� �� �ִ�.

#define PAGES_PER_TABLE		1024
#define PAGES_PER_DIRECTORY	1024
#define PAGE_TABLE_SIZE		4096

//! page table represents 4mb address space
#define PTABLE_ADDR_SPACE_SIZE 0x400000
//! directory table represents 4gb address space
#define DTABLE_ADDR_SPACE_SIZE 0x100000000
//! page sizes are 4k
#define PAGE_SIZE 4096

// chapter 21
#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

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
		
	bool Initialize();

	bool AllocPage(PTE* e);
	void FreePage(PTE* e);

	bool SwitchPageDirectory(PageDirectory* dir);

	PageDirectory* GetCurPageDirectory();

	//! flushes a cached translation lookaside buffer (TLB) entry
	void FlushTranslationLockBufferEntry(uint32_t addr);
	
//�����ؾ� ��
	void ClearPageTable(PageTable* p);	
	PTE* GetPTE(PageTable* p, uint32_t addr);
	uint32_t GetPageTableEntryIndex(uint32_t addr);
	
	uint32_t GetPageTableIndex(uint32_t addr);
	void ClearPageDirectory(PageDirectory* dir);	
	PDE* GetPDE(PageDirectory* p, uint32_t addr);
//

	bool CreatePageTable(PageDirectory* dir, uint32_t virt, uint32_t flags);
	void MapPhysicalAddressToVirtualAddresss(PageDirectory* dir, uint32_t virt, uint32_t phys, uint32_t flags);
	void* GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress);
	void UnmapPageTable(PageDirectory* dir, uint32_t virt);
	void UnmapPhysicalAddress(PageDirectory* dir, uint32_t virt);
	PageDirectory* CreateAddressSpace();

//Create Kernel Heap Physical Memory
	bool CreateKernelHeap();

	bool MapHeapSpace(PageDirectory* pDir);

	static VirtualMemoryManager* GetInstance()
	{
		return &m_virtualMemoryManager;
	}

private:
	static VirtualMemoryManager m_virtualMemoryManager;

	//! current directory table
	PageDirectory*		_cur_directory = 0;

	//! current page directory base register
	uint32_t	_cur_pdbr = 0;

	//Physical Heap Address
	void* m_pKernelHeapPhysicalMemory = 0;
};

