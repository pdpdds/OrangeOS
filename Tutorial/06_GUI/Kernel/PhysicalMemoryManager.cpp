#include "PhysicalMemoryManager.h"
#include "string.h"
#include "Console.h"

#ifdef _ORANGE_DEBUG
extern Console console;
#endif // ORANGE_DEBUG

PhysicalMemoryManager PhysicalMemoryManager::m_physicsMemoryManager;

PhysicalMemoryManager::PhysicalMemoryManager()
	: m_usedBlocks(0)
{
	
}


PhysicalMemoryManager::~PhysicalMemoryManager()
{
}

// memorySize : ��ü �޸��� ũ��(����Ʈ ������)
//bitmapAddr : Ŀ�δ����� ��ġ�Ǵ� ��Ʈ�� �迭
//�� �迭�� �����ؼ� �ش� ���� �޸𸮰� �Ҵ�Ǿ����� ����������� �Ǵ��Ѵ�.
void PhysicalMemoryManager::Initialize(size_t memorySize, uint32_t bitmapAddr)
{
	m_memorySize = memorySize;
	m_pMemoryMap = (uint32_t*)bitmapAddr;
	//m_maxBlocks = (pmmngr_get_memory_size() * 1024) / PMM_BLOCK_SIZE;
	m_maxBlocks = m_memorySize / PMM_BLOCK_SIZE;

	//������ �ִ� ���� 8�� ����� ���߰� �������� ������
	//m_maxBlocks = m_maxBlocks - (m_maxBlocks % PMM_BLOCKS_PER_BYTE);

	//�޸𸮸��� ����Ʈũ��
	m_memoryMapSize = m_maxBlocks / PMM_BLOCKS_PER_BYTE;
	m_usedBlocks = GetTotalBlockCount();
	
	//��� �޸� ������ ����߿� �ִٰ� �����Ѵ�.	
	memset((char*)m_pMemoryMap, 0xFF, m_memoryMapSize);
}

//8��° �޸� ���� ��������� ǥ���ϱ� ���� 1�� �����Ϸ��� �迭 ù��° ���(4����Ʈ) ����Ʈ�� 8��° ��Ʈ�� �����ؾ� �Ѵ�
void PhysicalMemoryManager::SetBit(int bit)
{			
	m_pMemoryMap[bit / 32] |= (1 << (bit % 32));		
}

//8��° �޸� ���� ����� �� ������ ǥ���ϱ� ���� 0���� �����Ϸ��� �迭 ù��° ���(4����Ʈ) ����Ʈ�� 8��° ��Ʈ�� �����ؾ� �Ѵ�
//00000001000000000000000000000000
//11111110111111111111111111111111
//AND �����ڿ� ���� 8��° ��Ʈ�� ������ ��Ʈ�� ��� ���� ���� �����Ѵ�
void PhysicalMemoryManager::UnsetBit(int bit)
{			
	m_pMemoryMap[bit / 32] &= ~(1 << (bit % 32));	
}

void PhysicalMemoryManager::LoadPDBR(uint32_t physicalAddr)
{
#ifdef _MSC_VER
	_asm 
	{
		mov	eax, [physicalAddr]
		mov	cr3, eax		// PDBR is cr3 register in i86
	}
#endif
}

uint32_t PhysicalMemoryManager::GetPDBR()
{

#ifdef _MSC_VER
	_asm 
	{
		mov	eax, cr3
		ret
	}
#endif
}

bool PhysicalMemoryManager::IsPaging()
{
	uint32_t res = 0;

#ifdef _MSC_VER
	_asm {
		mov	eax, cr0
		mov[res], eax
	}
#endif

	return (res & 0x80000000) ? false : true;
}

void PhysicalMemoryManager::EnablePaging(bool state)
{
#ifdef _MSC_VER
	_asm 
	{
		mov	eax, cr0
		cmp[state], 1
		je	enable
		jmp disable
		enable :
		or eax, 0x80000000		//set bit 31
			mov	cr0, eax
			jmp done
		disable :
		and eax, 0x7FFFFFFF		//clear bit 31
			mov	cr0, eax
			done :
	}
#endif
}

void* PhysicalMemoryManager::AllocBlock() {

	if (GetFreeBlockCount() <= 0)
		return NULL;

	int frame = GetFreeFrame();

	if (frame == -1)
		return NULL;

	SetBit(frame);

	uint32_t addr = frame * PMM_BLOCK_SIZE;
	m_usedBlocks++;

	return (void*)addr;
}

void PhysicalMemoryManager::FreeBlock(void* p) {

	uint32_t addr = (uint32_t)p;
	int frame = addr / PMM_BLOCK_SIZE;

	UnsetBit(frame);

	m_usedBlocks--;
}

void* PhysicalMemoryManager::AllocBlocks(size_t size)
{
	if (GetFreeBlockCount() <= size)
	{
		return NULL;
	}

	int frame = GetFreeFrames(size);

	if (frame == -1)
	{
		return NULL;	//���ӵ� �� ������ �������� �ʴ´�.
	}

	for (uint32_t i = 0; i < size ; i++)
		SetBit(frame + i);

	uint32_t addr = frame * PMM_BLOCK_SIZE;
	m_usedBlocks += size;
	
	return (void*)addr;
}

void PhysicalMemoryManager::FreeBlocks(void* p, size_t size) {

	uint32_t addr = (uint32_t)p;
	int frame = addr / PMM_BLOCK_SIZE;
	
	for (uint32_t i = 0; i<size ; i++)
		UnsetBit(frame + i);

	m_usedBlocks -= size;
}

size_t PhysicalMemoryManager::GetMemorySize() {

	return m_memorySize;
}

uint32_t PhysicalMemoryManager::GetUsedBlockCount() {

	return m_usedBlocks;
}

uint32_t PhysicalMemoryManager::GetFreeBlockCount() {

	return m_maxBlocks - m_usedBlocks;
}

uint32_t PhysicalMemoryManager::GetTotalBlockCount() {

	return m_maxBlocks;
}

uint32_t PhysicalMemoryManager::GetBlockSize()
{
	return PMM_BLOCK_SIZE;
}

//�ش� ��Ʈ�� ��Ʈ�Ǿ� �ִ��� �Ǿ� ���� �������� üũ�Ѵ�
//��Ʈ�� ��ȿ���� ����� 0�� �����Ѵ�.
bool PhysicalMemoryManager::TestMemoryMap(int bit)
{	
	return (m_pMemoryMap[bit / 32] & (1 << (bit % 32))) > 0;
}

//��Ʈ�� 0�� ������ �ε����� ����(����� �� �ִ� �� �� ��Ʈ �ε���)
int PhysicalMemoryManager::GetFreeFrame()
{	
	for (uint32_t i = 0; i < GetTotalBlockCount() / 32; i++)
	{
		if (m_pMemoryMap[i] != 0xffffffff)
			for (int j = 0; j < PMM_BITS_PER_INDEX; j++)
			{				
				int bit = 1 << j;
				if ((m_pMemoryMap[i] & bit) == 0)
					return i * PMM_BITS_PER_INDEX + j;
			}
	}

	return -1;
}

//���ӵ� �� ������(��)���� ����
int PhysicalMemoryManager::GetFreeFrames(size_t size)
{
	if (size == 0)
		return -1;

	if (size == 1)
		return GetFreeFrame();

	for (uint32_t i = 0; i < GetTotalBlockCount() / 32; i++)
		if (m_pMemoryMap[i] != 0xffffffff)
		{
			for (int j = 0; j < 32; j++)
			{
				int bit = 1 << j;
				if ((m_pMemoryMap[i] & bit) == 0) 
				{
					
					int startingBit = i * PMM_BITS_PER_INDEX + j;					

					// ���ӵ� �� �������� ������ ������Ų��.

					uint32_t free = 0; 
					for (uint32_t count = 0; count < size; count++)
					{
//�޸𸮸��� ����� ��Ȳ
						if (startingBit + count >= m_maxBlocks)
							return -1;

						if (TestMemoryMap(startingBit + count) == false)
							free++;	

						//���ӵ� �� �����ӵ��� �����Ѵ�. ���� ��Ʈ �ε����� startingBit
						if (free == size)
							return startingBit;
					}
				}
			}
		}

	return -1;
}

void PhysicalMemoryManager::InitMemoryRegion(uint32_t base, size_t size)
{
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	for (; blocks>0; blocks--) {
		UnsetBit(align++);
		m_usedBlocks--;
	}

	SetBit(0);	//first block is always set. This insures allocs cant be 0
}
void PhysicalMemoryManager::DeinitMemoryRegion(uint32_t base, size_t size)
{
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	for (; blocks>0; blocks--) {
		SetBit(align++);
		m_usedBlocks++;
	}

	SetBit(0);	//first block is always set. This insures allocs cant be 0

}

void PhysicalMemoryManager::Dump()
{
#ifdef _ORANGE_DEBUG
	console.Print("Physical Memory Manager Init..\n");
	console.Print("Memory Size : 0x%x\n", m_memorySize);
	console.Print("Memory Map Address : 0x%x\n", m_pMemoryMap);
	console.Print("Memory Map Size : 0x%x\n", m_memoryMapSize);
	console.Print("Max Block Count : 0x%x\n", m_maxBlocks);

	console.Print("Used Block Count : 0x%x\n", m_usedBlocks);	
#endif // _ORANGE_DEBUG
}