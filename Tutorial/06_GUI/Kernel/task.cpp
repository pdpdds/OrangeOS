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
#include "pit.h"

extern Console console;

//32��Ʈ PE���� �̹��� ��ȿ�� �˻�
bool ValidatePEImage(void* image) 
{
    IMAGE_DOS_HEADER* dosHeader = 0;
    IMAGE_NT_HEADERS* ntHeaders = 0;
    
    dosHeader = (IMAGE_DOS_HEADER*)image;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)             
		return false;
    
	if (dosHeader->e_lfanew == 0)
		return false;
    
    //NT Header üũ
    ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)image);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)            
		return false;    

    /* only supporting for i386 archs */
    if (ntHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_I386)             
		return false;    

    /* only support 32 bit executable images */
    if (! (ntHeaders->FileHeader.Characteristics & (IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_32BIT_MACHINE)))             
		return false;
    
    /*
            Note: 1st 4 MB remains idenitity mapped as kernel pages as it contains
            kernel stack and page directory. If you want to support loading below 1MB,
            make sure to move these into kernel land
    */

	//�ε�Ǵ� ���μ����� ���̽� �ּҴ� 0x00400000��. 
	//����� ��Ʃ������� �Ӽ�=> ��Ŀ => ����� �����ּ� �׸񿡼� Ȯ�� �����ϴ�
    if ( (ntHeaders->OptionalHeader.ImageBase < 0x400000) || (ntHeaders->OptionalHeader.ImageBase > 0x80000000))
		return false;
    
    /* only support 32 bit optional header format */
    if (ntHeaders->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) 
            return false;

//��ȿ�� 32��Ʈ PE �����̴�.
	return true;
}

extern "C" 
{
	uint32_t MemoryAlloc(size_t size)
	{
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		void *addr = alloc(size, (u8int)0, (heap_t*)pProcess->m_lpHeap);

#ifdef _ORANGE_DEBUG
		console.Print("process heap alloc, %d %x\n", size, pProcess->m_lpHeap);
		console.Print("process heap alloc, %d %x\n", size, pProcess->m_lpHeap);
#endif			
		return (u32int)addr;
	}

	void MemoryFree(void* p)
	{
		//���� �����尡 ��� �����Ѵ�.
		//���� �޸𸮸� �����ҽ� ���ؽ�Ʈ ����Ī�� �Ͼ�� �ٸ� �����尡 ���� �ڿ�(��)�� ������ �� �ִ� ���ɼ��� ����Ƿ�
		//���ͷ�Ʈ�� �Ͼ�� �ʰ� ó���Ѵ�.
		EnterCriticalSection();
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		free(p, (heap_t*)pProcess->m_lpHeap);
		LeaveCriticalSection();
	}

	//���μ��� ������ ����Ʈ ���� �����Ѵ�
	void CreateDefaultHeap() 
	{
		EnterCriticalSection();
		
		Process* pProcess = ProcessManager::GetInstance()->GetCurrentProcess();
		Thread* pThread = pProcess->GetThread(0);
		
	//1�ް� ����Ʈ�� ���� ����
		void* pHeapPhys = PhysicalMemoryManager::GetInstance()->AllocBlocks(DEFAULT_HEAP_PAGE_COUNT);
		u32int heapAddess = pThread->m_imageBase + pThread->m_imageSize + PAGE_SIZE + PAGE_SIZE * 2;
		
	//�� �ּҸ� 4K�� ���� Align	
		heapAddess -= (heapAddess % PAGE_SIZE);
		
#ifdef _ORANGE_DEBUG
		console.Print("heap adress %x\n", heapAddess);
#endif // _ORANGE_DEBUG
				
		for (int i = 0; i < DEFAULT_HEAP_PAGE_COUNT; i++)
		{
			VirtualMemoryManager::GetInstance()->MapPhysicalAddressToVirtualAddresss(pProcess->m_pPageDirectory,
				(uint32_t)heapAddess + i * PAGE_SIZE,
				(uint32_t)pHeapPhys + i * PAGE_SIZE,
				I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);
		}
		
		memset((void*)heapAddess, 0, DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE);
		
		pProcess->m_lpHeap = create_heap((u32int)heapAddess, (uint32_t)heapAddess + DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE, 
			                             (uint32_t)heapAddess + DEFAULT_HEAP_PAGE_COUNT * PAGE_SIZE, 0, 0);

		LeaveCriticalSection();		
	}
	
	//���μ��� ����	
	extern "C" void TerminateProcess()
	{
		EnterCriticalSection();

		Process* cur = ProcessManager::GetInstance()->GetCurrentProcess();

		if (cur == NULL || cur->m_processId == PROC_INVALID_ID)
		{
			console.Print("Invailid Process Termination\n");
			return;
		}

		//���μ��� �Ŵ������� �ش� ���μ����� ������ �����Ѵ�.
		//�½�ũ ��Ͽ����� ���ŵǾ� �ش� ���μ����� ���̻� �����층 ���� �ʴ´�.
		ProcessManager::GetInstance()->DestroyProcess(cur);

		LeaveCriticalSection();

		for (;;);
	}	
}
