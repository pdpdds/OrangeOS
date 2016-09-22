#pragma once
#include "windef.h"
#include "stdint.h"

class PhysicalMemoryManager
{
public:
#define PMM_BLOCKS_PER_BYTE 8	
#define PMM_BLOCK_SIZE	4096	
#define PMM_BLOCK_ALIGN	BLOCK_SIZE
#define PMM_BITS_PER_INDEX	32

public:
	PhysicalMemoryManager();
	virtual ~PhysicalMemoryManager();
	
	void	Initialize(size_t memorySize, uint32_t bitmapAddr);

	//물리 메모리의 사용 여부에 따라 초기에 프레임들을 Set하거나 Unset한다	
	void	InitMemoryRegion(uint32_t, size_t);	
	void	DeinitMemoryRegion(uint32_t base, size_t);
	
	void*	AllocBlock();	
	void	FreeBlock(void*);

	void*	AllocBlocks(size_t);
	void	FreeBlocks(void*, size_t);	

	size_t GetMemorySize();	

	int GetFreeFrame();
	int GetFreeFrames(size_t size);

	uint32_t GetUsedBlockCount();	
	uint32_t GetFreeBlockCount();
	
	uint32_t GetTotalBlockCount();
	uint32_t GetBlockSize();
	
	bool TestMemoryMap(int bit);
		
	void EnablePaging(bool state);
	bool	IsPaging();
		
	void LoadPDBR(uint32_t physicalAddr);
	uint32_t GetPDBR();	

	static PhysicalMemoryManager* GetInstance()
	{
		return &m_physicsMemoryManager;
	}
//Debug
	void Dump();

private:
	static PhysicalMemoryManager m_physicsMemoryManager;
	
	uint32_t	m_memorySize = 0;
	uint32_t	m_usedBlocks = 0;

	//이용할 수 있는 최대 블럭 갯수
	uint32_t	m_maxBlocks = 0;

	//비트맵 배열, 각 비트는 메모리 블럭을 표현, 비트맵처리
	uint32_t*	m_pMemoryMap = 0;
	uint32_t	m_memoryMapSize = 0;

	void SetBit(int bit);
	void UnsetBit(int bit);	
};