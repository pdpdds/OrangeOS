#include "VirtualMemoryManager.h"
#include "PhysicalMemoryManager.h"
#include "string.h"
#include "Console.h"
#include "kheap.h"

VirtualMemoryManager VirtualMemoryManager::m_virtualMemoryManager;


#ifdef _ORANGE_DEBUG
extern Console console;
#endif // ORANGE_DEBUG


VirtualMemoryManager::VirtualMemoryManager()
{
}


VirtualMemoryManager::~VirtualMemoryManager()
{
}

//���� �ּҿ� ���ε� ���� ���� �ּҸ� ����.
void* VirtualMemoryManager::GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress)
{
	PDE* pagedir = directory->m_entries;
	if (pagedir[virtualAddress >> 22] == 0)
		return NULL;

	return (void*)((uint32_t*)(pagedir[virtualAddress >> 22] & ~0xfff))[virtualAddress << 10 >> 10 >> 12];
}

//������ ���丮 ��Ʈ�� �ε����� 0�� �ƴϸ� �̹� ������ ���̺��� �����Ѵٴ� �ǹ�
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

//PDE�� PTE�� �÷��״� ���� ���� ����
//�����ּҸ� ���� �ּҿ� ����
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

//���캼 ��
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


bool VirtualMemoryManager::Initialize() 
{	
	//! allocates 3gb page table
	PageTable* identityPageTable = (PageTable*)PhysicalMemoryManager::GetInstance()->AllocBlock();
	if (identityPageTable == NULL)
		return false;

#ifdef _ORANGE_DEBUG
	console.Print("Identity Page Table Alloc : 0x%x\n", identityPageTable);
#endif // _ORANGE_DEBUG
	

//0-4MB �� ���� �ּҸ� ���� �ּҿ� �����ϰ� ���ν�Ų��
	for (int i = 0, frame = 0x0, virt = 0x00000000; i<PAGES_PER_TABLE; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
	{
		PTE page = 0;
		PageTableEntry::AddAttribute(&page, I86_PTE_PRESENT);
		PageTableEntry::SetFrame(&page, frame);
			
		identityPageTable->m_entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	//������ ���丮 ����. 4GB�� ǥ���ϱ� ���ؼ� ������ ���丮�� �ϳ��� ����ϴ�.
	PageDirectory* dir = (PageDirectory*)PhysicalMemoryManager::GetInstance()->AllocBlock();

	if (dir == NULL)
		return false;

#ifdef _ORANGE_DEBUG
	console.Print("Page Directory Alloc : 0x%x\n", dir);
#endif // _ORANGE_DEBUG
	
	memset(dir, 0, sizeof(PageDirectory));
	
//���� ���� 1MB ������ ������ �����ּ� ���� 0XC0000000(3GB) ������ ������ ���ν�Ų��
//Ŀ���� �ε�� ���� ��巹�� �ּ� 0x100000
// �����ּ� 0XC0000000, 0X100000�� �����ּ� 0x100000�� ���εȴ�
//���� Ŀ���� ���� PDE, PTE
	for (int y = 0; y < 1; y++)
	{
		PageTable* pTable = (PageTable*)PhysicalMemoryManager::GetInstance()->AllocBlock();
		if (!pTable)
			return false;

#ifdef _ORANGE_DEBUG
		console.Print("Page Table For Kernel Mapping: 0x%x\n", pTable);
#endif // _ORANGE_DEBUG

		//! clear page table
		memset(pTable, 0, sizeof(PageTable));

		int virt = KERNEL_VIRTUAL_BASE_ADDRESS + y * PAGES_PER_TABLE * PAGE_SIZE;

//Ŀ���� ������� 4�ް��� ���� �ʴ´ٰ� �����Ѵ�
		int frame = KERNEL_PHYSICAL_BASE_ADDRESS;

		for (int i = 0; i < PAGES_PER_TABLE; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
		{			
			PTE page = 0;
			PageTableEntry::AddAttribute(&page, I86_PTE_PRESENT);
			PageTableEntry::SetFrame(&page, frame);
		
			pTable->m_entries[PAGE_TABLE_INDEX(virt)] = page;
		}

		//! get first entry in dir table and set it up to point to our table
		PDE* entry = &dir->m_entries[PAGE_DIRECTORY_INDEX(KERNEL_VIRTUAL_BASE_ADDRESS + y * PAGES_PER_TABLE * PAGE_SIZE)];
		PageDirectoryEntry::AddAttribute(entry, I86_PDE_PRESENT);
		PageDirectoryEntry::AddAttribute(entry, I86_PDE_WRITABLE);
		PageDirectoryEntry::SetFrame(entry, (uint32_t)pTable);
	}	

	PDE* identityEntry = &dir->m_entries[PAGE_DIRECTORY_INDEX(0x00000000)];
	PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_PRESENT);
	PageDirectoryEntry::AddAttribute(identityEntry, I86_PDE_WRITABLE);
	PageDirectoryEntry::SetFrame(identityEntry, (uint32_t)identityPageTable);	

	if (false == SetPageDirectoryInfo(dir))
		return false;

	_asm
	{
		mov	eax, [dir]
		mov	cr3, eax		// PDBR is cr3 register in i86
	}

#ifdef _ORANGE_DEBUG
	console.Print("Current Page Directory Base Register : 0x%x\n", _cur_pdbr);
#endif // _ORANGE_DEBUG	

	CreateKernelHeap(GetCurPageDirectory());
	
	
	PhysicalMemoryManager::GetInstance()->EnablePaging(true);


	return true;
}

bool VirtualMemoryManager::SetPageDirectoryInfo(PageDirectory* dir)
{
	if (dir == NULL)
		return false;

	_cur_directory = dir;
	_cur_pdbr = (uint32_t)&dir->m_entries;	
	
	return true;
}

PageDirectory* VirtualMemoryManager::GetCurPageDirectory() 
{
	return _cur_directory;
}

void VirtualMemoryManager::FlushTranslationLockBufferEntry(uint32_t addr)
{
#ifdef _MSC_VER
	_asm {
		cli
		invlpg	addr
		sti
	}
#endif
}

//256 * 4096 = 1MB, 1MB�� ���� �Ҵ��Ѵ�

bool VirtualMemoryManager::CreateKernelHeap(PageDirectory* dir)
{	
	//Virtual Heap Address
	void* pVirtualHeap = (void*)(KERNEL_VIRTUAL_HEAP_ADDRESS);

	m_pKernelHeapPhysicalMemory = PhysicalMemoryManager::GetInstance()->AllocBlocks(256);

	if (m_pKernelHeapPhysicalMemory == NULL)
		return false;

	MapHeap(dir);
			
	create_kernel_heap((u32int)pVirtualHeap, (uint32_t)pVirtualHeap + 256 * PAGE_SIZE, (uint32_t)pVirtualHeap + 256 * PAGE_SIZE, 0, 0);		

	return true;
}

bool VirtualMemoryManager::MapHeap(PageDirectory* dir)
{
	//Virtual Heap Address
	void* pVirtualHeap = (void*)(KERNEL_VIRTUAL_HEAP_ADDRESS);

	for (int i = 0; i < 256; i++)
	{
		VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(dir, (uint32_t)pVirtualHeap + i * PAGE_SIZE, (uint32_t)m_pKernelHeapPhysicalMemory + i * PAGE_SIZE, I86_PTE_PRESENT | I86_PTE_WRITABLE);
	}

	return true;
}