#include "PhysicalMemoryManager.h"
#include "string.h"
#include "Console.h"

#ifdef _ORANGE_DEBUG

#endif // ORANGE_DEBUG
extern Console console;

PhysicalMemoryManager PhysicalMemoryManager::m_physicsMemoryManager;

PhysicalMemoryManager::PhysicalMemoryManager()
	: m_usedBlocks(0)
{
	
}


PhysicalMemoryManager::~PhysicalMemoryManager()
{
}

// memorySize : 전체 메모리의 크기(바이트 사이즈)
//bitmapAddr : 커널다음에 배치되는 비트맵 배열
//이 배열을 참조해서 해당 물리 메모리가 할당되었는지 사용중인지를 판단한다.
void PhysicalMemoryManager::Initialize(size_t memorySize, uint32_t bitmapAddr)
{
	m_memorySize = memorySize;
	m_maxBlocks = m_memorySize / PMM_BLOCK_SIZE;
	m_pMemoryMap = (uint32_t*)bitmapAddr;	

	//블럭들의 최대 수는 8의 배수로 맞추고 나머지는 버린다
	//m_maxBlocks = m_maxBlocks - (m_maxBlocks % PMM_BLOCKS_PER_BYTE);

	//메모리맵의 바이트크기
	m_memoryMapSize = m_maxBlocks / PMM_BLOCKS_PER_BYTE;
	m_usedBlocks = GetTotalBlockCount();
	
	//모든 메모리 블럭들이 사용중에 있다고 설정한다.	
	memset((char*)m_pMemoryMap, 0x00, m_memoryMapSize);
}

//8번째 메모리 블럭이 사용중임을 표시하기 위해 1로 세팅하려면 배열 첫번째 요소(4바이트) 바이트의 8번째 비트에 접근해야 한다
void PhysicalMemoryManager::SetBit(int bit)
{			
	m_pMemoryMap[bit / 32] |= (1 << (bit % 32));		
}

//8번째 메모리 블럭을 사용할 수 있음을 표시하기 위해 0으로 세팅하려면 배열 첫번째 요소(4바이트) 바이트의 8번째 비트에 접근해야 한다
//00000001000000000000000000000000
//11111110111111111111111111111111
//AND 연산자에 의해 8번째 비트를 제외한 비트는 모두 원래 값을 유지한다
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
		return NULL;	//연속된 빈 블럭들이 존재하지 않는다.
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

//해당 비트가 세트되어 있는지 되어 있지 않은지를 체크한다
//비트가 유효값을 벗어나면 0을 리턴한다.
bool PhysicalMemoryManager::TestMemoryMap(int bit)
{	
	return (m_pMemoryMap[bit / 32] & (1 << (bit % 32))) > 0;
}

//비트가 0인 프레임 인덱스를 얻어낸다(사용할 수 있는 빈 블럭 비트 인덱스)
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

//연속된 빈 프레임(블럭)들을 얻어낸다
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

					// 연속된 빈 프레임의 갯수를 증가시킨다.

					uint32_t free = 0; 
					for (uint32_t count = 0; count < size; count++)
					{
//메모리맵을 벗어나는 상황
						if (startingBit + count >= m_maxBlocks)
							return -1;

						if (TestMemoryMap(startingBit + count) == false)
							free++;	

						//연속된 빈 프레임들이 존재한다. 시작 비트 인덱스는 startingBit
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

	//SetBit(0);	//first block is always set. This insures allocs cant be 0
}
void PhysicalMemoryManager::DeinitMemoryRegion(uint32_t base, size_t size)
{
	int align = base / PMM_BLOCK_SIZE;
	int blocks = size / PMM_BLOCK_SIZE;

	for (; blocks>0; blocks--) {
		SetBit(align++);
		m_usedBlocks++;
	}

	//SetBit(0);	//first block is always set. This insures allocs cant be 0

}

void PhysicalMemoryManager::Dump()
{
//#ifdef _ORANGE_DEBUG
	console.Print("Physical Memory Manager Init..\n");
	console.Print("Memory Size : 0x%x\n", m_memorySize);
	console.Print("Memory Map Address : 0x%x\n", m_pMemoryMap);
	console.Print("Memory Map Size : 0x%x\n", m_memoryMapSize);
	console.Print("Max Block Count : 0x%x\n", m_maxBlocks);

	console.Print("Used Block Count : 0x%x\n", m_usedBlocks);	
//#endif // _ORANGE_DEBUG
}