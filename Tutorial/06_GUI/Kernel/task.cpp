//****************************************************************************
//**
//**    task.cpp
//**		-Task Manager
//**
//****************************************************************************
//============================================================================
//    IMPLEMENTATION HEADERS
//============================================================================

#include <string.h>
#include "fsys.h"
#include "image.h"
#include "VirtualMemoryManager.h"
#include "PhysicalMemoryManager.h"
#include "task.h"
#include "Console.h"
#include "ProcessManager.h"
#include "fsys.h"
#include "defines.h"

//============================================================================
//    IMPLEMENTATION PRIVATE DEFINITIONS / ENUMERATIONS / SIMPLE TYPEDEFS
//============================================================================

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

extern Console console;

//============================================================================
//    IMPLEMENTATION PRIVATE CLASS PROTOTYPES / EXTERNAL CLASS REFERENCES
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE STRUCTURES / UTILITY CLASSES
//============================================================================
//============================================================================
//    IMPLEMENTATION REQUIRED EXTERNAL REFERENCES (AVOID)
//============================================================================
//============================================================================
//    IMPLEMENTATION PRIVATE DATA
//============================================================================


/**
* Validate image
* \param image Base of image
* \ret Status code
*/
int validateImage (void* image) {
    IMAGE_DOS_HEADER* dosHeader = 0;
    IMAGE_NT_HEADERS* ntHeaders = 0;

    /* validate program file */
    dosHeader = (IMAGE_DOS_HEADER*) image;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
            return 0;
    }
    if (dosHeader->e_lfanew == 0) {
            return 0;
    }

    /* make sure header is valid */
    ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)image);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
            return 0;
    }

    /* only supporting for i386 archs */
    if (ntHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_I386) {
            return 0;
    }

    /* only support 32 bit executable images */
    if (! (ntHeaders->FileHeader.Characteristics &
            (IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE))) {
            return 0;
    }
    /*
            Note: 1st 4 MB remains idenitity mapped as kernel pages as it contains
            kernel stack and page directory. If you want to support loading below 1MB,
            make sure to move these into kernel land
    */
    if ( (ntHeaders->OptionalHeader.ImageBase < 0x400000)
            || (ntHeaders->OptionalHeader.ImageBase > 0x80000000)) {
            return 0;
    }

    /* only support 32 bit optional header format */
    if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
            return 0;
    }
	return 1;
}

/* kernel command shell */
extern void run ();

/**
* TerminateProcess system call
*/
extern "C" {
void TerminateProcess () {
	
	_asm
	{
		cli
	}

	Process* cur = ProcessManager::GetInstance()->GetCurrentProcess();
	
	if (cur->m_taskId == PROC_INVALID_ID)
	{
		console.Print("Invailid Process Id\n");
		return;
	}

	/* release threads */
	
	//Thread* pThread = ProcessManager::GetInstance()->g_pThread;
	Thread* pThread = cur->GetThread(0);
	cur->DelThread(0);	

	u32int heapAddess = pThread->imageBase + pThread->imageSize + PAGE_SIZE + PAGE_SIZE * 2;
	heapAddess = heapAddess - (heapAddess % PAGE_SIZE);
	for (int i = 0; i < 300; i++)
	{
		uint32_t phys = 0;
		uint32_t virt = 0;

		/* get virtual address of page */
		virt = heapAddess + (i * PAGE_SIZE);
		
		/* unmap and release page */
		VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(cur->pPageDirectory, virt);		
	}

	/* unmap and release image memory */
	for (uint32_t page = 0; page < cur->dwPageCount; page++) {
		uint32_t phys = 0;
		uint32_t virt = 0;

		/* get virtual address of page */
		virt = pThread->imageBase + (page * PAGE_SIZE);

		/* get physical address of page */
		phys = (uint32_t)VirtualMemoryManager::GetInstance()->GetPhysicalAddressFromVirtualAddress(cur->pPageDirectory, virt);

		/* unmap and release page */
		VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(cur->pPageDirectory, virt);
		//pmmngr_free_block ((void*)phys);
	}

	/* get physical address of stack */
	void* stackFrame = VirtualMemoryManager::GetInstance()->GetPhysicalAddressFromVirtualAddress(cur->pPageDirectory,
		(uint32_t)pThread->initialStack);

	/* unmap and release stack memory */
	VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(cur->pPageDirectory, (uint32_t)pThread->initialStack);
	//pmmngr_free_block(stackFrame);

	PhysicalMemoryManager::GetInstance()->FreeBlock(cur->pPageDirectory);	

	Orange::LinkedList *pProcessList = ProcessManager::GetInstance()->GetProcessList();
	
	pProcessList->Delete(cur);	
	console.Print("terminate %s\n", cur->processName);
	
	delete pThread;
	delete cur;

	Process* pProcess = (Process*)pProcessList->Get(0);
	ProcessManager::GetInstance()->SetCurrentProcess(pProcess);
	pProcess->dwRunState = PROCESS_STATE_SLEEP;

	console.Print("next processs %x\n", pProcess);

	/* restore kernel selectors */
	/*__asm {
		cli
		mov eax, 0x10
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
		sti
	}*/

	for (;;);

	//pmmngr_load_PDBR((physical_addr)vmmngr_get_directory());	

	//run ();
	
	for (;;);
}
} // extern "C"

//============================================================================
//    INTERFACE CLASS BODIES
//============================================================================
//****************************************************************************
//**
//**    END[task.cpp]
//**
//****************************************************************************
extern "C" {
	void TerminateMemoryProcess() {
		
		Orange::LinkedList *pProcessList = ProcessManager::GetInstance()->GetProcessList();
		Process* pProcess = (Process*)pProcessList->Get(0);

		if (pProcess->m_taskId == PROC_INVALID_ID)
		{
			console.Print("Invalid Memory Process Id\n");
			return;
		}

		/* release threads */
		int i = 0;

		Thread* pThread = pProcess->GetThread(0);
		
		/* get physical address of stack */
		
		void* stackFrame = VirtualMemoryManager::GetInstance()->GetPhysicalAddressFromVirtualAddress(pProcess->pPageDirectory,
			(uint32_t)pThread->initialStack);

		console.Print("Plane X : %d, Plane Y : %d\n", 1, 1);
		/* unmap and release stack memory */
		
		VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(pProcess->pPageDirectory, (uint32_t)pThread->initialStack);
		//pmmngr_free_block(stackFrame);

		/* unmap and release image memory */
		for (uint32_t page = 0; page < pThread->imageSize / PAGE_SIZE; page++) {
			uint32_t phys = 0;
			uint32_t virt = 0;

			/* get virtual address of page */
			virt = pThread->imageBase + (page * PAGE_SIZE);

			/* get physical address of page */
			phys = (uint32_t)VirtualMemoryManager::GetInstance()->GetPhysicalAddressFromVirtualAddress(pProcess->pPageDirectory, virt);

			/* unmap and release page */
			VirtualMemoryManager::GetInstance()->UnmapPhysicalAddress(pProcess->pPageDirectory, virt);
			//pmmngr_free_block((void*)phys);
		}		

		/* restore kernel selectors */
		__asm {
			cli
				mov eax, 0x10
				mov ds, ax
				mov es, ax
				mov fs, ax
				mov gs, ax
				sti
		}
		
		PhysicalMemoryManager::GetInstance()->LoadPDBR((uint32_t)pProcess->pPageDirectory);

		/* return to kernel command shell */
		//run();

		console.Print("Exit command recieved; demo halted\n");
		for (;;);
	}
} // extern "C"

extern "C" {
	uint32_t MemoryAlloc(size_t size) {
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		Thread* pThread = pProcess->GetThread(0);
		console.Print("process heap alloc, %d %x\n", size, pThread->lpHeap);
		void *addr = alloc(size, (u8int)0, (heap_t*)pThread->lpHeap);
		console.Print("process heap alloc, %d %x\n", size, pThread->lpHeap);
		
		return (u32int)addr;
	}
} // extern "C"


extern "C" {
	void MemoryFree(void* p) {
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		Thread* pThread = pProcess->GetThread(0);
		free(p, (heap_t*)pThread->lpHeap);
	}
} // extern "C"
#include "pit.h"
extern "C" {
	uint32_t GetSysytemTickCount() {
		return GetTickCount();
	}
} // extern "C"


extern "C" {
	void CreateDefaultHeap() {

		InterruptDisable();
		
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		Thread* pThread = pProcess->GetThread(0);
		
		void* pHeapPhys = PhysicalMemoryManager::GetInstance()->AllocBlocks(256);
		
		u32int heapAddess = pThread->imageBase + pThread->imageSize + PAGE_SIZE + PAGE_SIZE * 2;
		heapAddess = heapAddess - (heapAddess % PAGE_SIZE);
		//u32int heapAddess = 0xB0000000;
		console.Print("heap adress %x\n", heapAddess);
		
		for (int i = 0; i < 300; i++)
		{
			VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(pProcess->pPageDirectory,
				(uint32_t)heapAddess + i * PAGE_SIZE,
				(uint32_t)pHeapPhys + i * PAGE_SIZE,
				I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);
		}
		
		memset((void*)heapAddess, 0, 300 * PAGE_SIZE);
		console.Print("imageSize %x\n", pThread->imageSize);
		pThread->lpHeap = create_heap((u32int)heapAddess, (uint32_t)heapAddess + 256 * 4096, (uint32_t)heapAddess + 256 * 4096, 0, 0);

		InterruptEnable();
		
	}
} // extern "C"
