#pragma once
#include "windef.h"
#include "stdint.h"

class VirtualMemoryManager
{
public:
	VirtualMemoryManager();
	virtual ~VirtualMemoryManager();

	void* GetPhysicalAddressFromVirtualAddress(pdirectory* dir, uint32_t virt);

	static VirtualMemoryManager* GetInstance()
	{
		return &m_virtualMemoryManager;
	}

private:
	static VirtualMemoryManager m_virtualMemoryManager;
};

