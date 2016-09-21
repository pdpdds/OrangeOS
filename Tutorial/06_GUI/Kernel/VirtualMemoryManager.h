#pragma once
#include "windef.h"
#include "stdint.h"


//! i86 architecture defines 1024 entries per table--do not change
#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR	1024

struct PageTable {

	pt_entry m_entries[PAGES_PER_TABLE];
};

//! page directory
struct PageDirectory {

	pd_entry m_entries[PAGES_PER_DIR];
};

class VirtualMemoryManager
{
public:
	VirtualMemoryManager();
	virtual ~VirtualMemoryManager();

	void* GetPhysicalAddressFromVirtualAddress(PageDirectory* directory, uint32_t virtualAddress);

	static VirtualMemoryManager* GetInstance()
	{
		return &m_virtualMemoryManager;
	}

private:
	static VirtualMemoryManager m_virtualMemoryManager;
};

