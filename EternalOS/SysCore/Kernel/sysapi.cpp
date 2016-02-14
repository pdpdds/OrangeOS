#include "sysapi.h"
#include "kheap.h"
#include "idt.h"

_declspec(naked) void syscall_dispatcher() 
{

	/* save index and set kernel data selector */
	static uint32_t idx = 0;
	_asm {
		push eax
			mov eax, 0x10
			mov ds, ax
			pop eax
			mov[idx], eax
			pusha
	}

	//! bounds check
	if (idx >= MAX_SYSCALL) {
		_asm {
			/* restore registers and return */
			popa
				iretd
		}
	}

	//! get service
	static void* fnct = 0;
	fnct = _syscalls[idx];

	//! and call service
	_asm {
		/* restore registers */
		popa
			/* call service */
			push edi
			push esi
			push edx
			push ecx
			push ebx
			call fnct
			add esp, 20
			/* restore usermode data selector */
			push eax
			mov eax, 0x23
			mov ds, ax
			pop eax
			iretd
	}
}

void InitializeSysCall() 
{
	setvect(0x80, syscall_dispatcher, I86_IDT_DESC_RING3);
}

void *operator new(size_t size)
{
	return (void *)kmalloc(size);
}

void *operator new[](size_t size)
{
	return (void *)kmalloc(size);
}

void operator delete(void *p)
{
	kfree(p);
}

int __cdecl _purecall()
{
	// Do nothing or print an error message.
	return 0;
}

void operator delete[](void *p)
{
	kfree(p);
}

bool InitializeMemorySystem(multiboot_info* bootinfo, uint32_t kernelSize)
{
	pmmngr_init((size_t)bootinfo->m_memorySize, 0xC0000000 + kernelSize * 512);

	memory_region*	region = (memory_region*)0x1000;

	for (int i = 0; i<10; ++i) {

		if (region[i].type>4)
			break;

		if (i>0 && region[i].startLo == 0)
			break;

		pmmngr_init_region(region[i].startLo, region[i].sizeLo);
	}
	pmmngr_deinit_region(0x100000, kernelSize * 512 + 4096 * 1024 + 4096 * 1024);
	/*
	kernel stack location
	*/
	pmmngr_deinit_region(0x0, 0x10000);

	//! initialize our vmm
	vmmngr_initialize();

	CreateKernelHeap(kernelSize);

	return true;
}

void* pHeapPhys = 0;

void CreateKernelHeap(int kernelSize)
{
	void* pHeap =
		(void*)(0xC0000000 + 409600 + 1024 * 4096);

	pHeapPhys = (void*)pmmngr_alloc_blocks(1000);

	for (int i = 0; i < 1000; i++)
	{
		vmmngr_mapPhysicalAddress(vmmngr_get_directory(),
			(uint32_t)pHeap + i * 4096,
			(uint32_t)pHeapPhys + i * 4096,
			I86_PTE_PRESENT | I86_PTE_WRITABLE);
	}

	create_kernel_heap((u32int)pHeap, (uint32_t)pHeap + 1000 * 4096, (uint32_t)pHeap + 1000 * 4096, 0, 0);
}

//! sleeps a little bit. This uses the HALs get_tick_count() which in turn uses the PIT
void sleep(int ms) 
{

	static int ticks = ms + get_tick_count();
	while (ticks > get_tick_count())
		;
}