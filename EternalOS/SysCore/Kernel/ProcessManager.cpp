#include "ProcessManager.h"
#include "Process.h"
#include "task.h"
#include "Thread.h"
#include "DebugDisplay.h"
#include "string.h"

ProcessManager* ProcessManager::m_pProcessManager = 0;

ProcessManager::ProcessManager()
{
	m_nextProcessId = 1;
	g_pCurProcess = 0;
}

ProcessManager::~ProcessManager()
{
}

Process* ProcessManager::CreateMemoryProcess(void(*lpStartAddress)())
{
	Process* pProcess = new Process();
	pProcess->TaskID = 1;
	pProcess->pPageDirectory = vmmngr_createAddressSpace();
	mapKernelSpace(pProcess->pPageDirectory);

	pProcess->dwPriority = 1;
	pProcess->dwRunState = PROCESS_STATE_ACTIVE;
	
	Thread* pThread = CreateMemoryThread(pProcess, lpStartAddress);
	List_Add(&pProcess->pThreadQueue, "", pThread);
	//List_Add(&pProcessQueue, "", pProcess);

	DebugPrintf("\nCreate Success Memory Task");

	return pProcess;
}

#include "task.h"
#define PAGE_SIZE 4096
Thread* ProcessManager::CreateThread(Process* pProcess, FILE* file)
{
	unsigned char buf[512];
	IMAGE_DOS_HEADER* dosHeader = 0;
	IMAGE_NT_HEADERS* ntHeaders = 0;
	unsigned char* memory = 0;

	/* read 512 bytes into buffer */
	volReadFile(file, buf, 512);
	if (!validateImage(buf)) {
		volCloseFile(file);
		return 0;
	}

	dosHeader = (IMAGE_DOS_HEADER*)buf;
	ntHeaders = (IMAGE_NT_HEADERS*)(dosHeader->e_lfanew + (uint32_t)buf);

	Thread* pThread = new Thread();
	pThread->pParent = pProcess;

	pThread->kernelStack = 0;
	pThread->priority = 1;
	pThread->state = PROCESS_STATE_ACTIVE;
	pThread->initialStack = 0;
	pThread->stackLimit = (void*)((uint32_t)pThread->initialStack + 4096);
	pThread->imageBase = ntHeaders->OptionalHeader.ImageBase;
	pThread->imageSize = ntHeaders->OptionalHeader.SizeOfImage;
	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)ntHeaders->OptionalHeader.AddressOfEntryPoint + ntHeaders->OptionalHeader.ImageBase;
	pThread->frame.flags = 0x200;
	

	int pageRest = 0;

	if ((pThread->imageSize % 4096) > 0)
		pageRest = 1;

	pProcess->dwPageCount = (pThread->imageSize / 4096) + pageRest;

	memory = (unsigned char*)pmmngr_alloc_blocks(pProcess->dwPageCount);
	
	/* map page into address space */
	for (int i = 0; i <pProcess->dwPageCount; i++)
	{
		//DebugPrintf("\ndsfdsd");
		vmmngr_mapPhysicalAddress(pProcess->pPageDirectory,
			ntHeaders->OptionalHeader.ImageBase + i * 4096,
			(uint32_t)memory + i * 4096,
			I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);
	}
	memset(memory, 0, pThread->imageSize);
	memcpy(memory, buf, 512);

	/* load image into memory */

	int fileRest = 0;
	if ((pThread->imageSize % 512) != 0)
		fileRest = 1;

	int readCount = (pThread->imageSize / 512) + fileRest;
	for (int i = 1; i < readCount; i++) {
		if (file->eof == 1)
			break;
		volReadFile(file, memory + 512 * i, 512);
	}
		
	/* close file and return process ID */
	volCloseFile(file);

	/* Create userspace stack (process esp=0x100000) */
	void* stack = (void*)(ntHeaders->OptionalHeader.ImageBase + ntHeaders->OptionalHeader.SizeOfImage + PAGE_SIZE);
	void* stackPhys = (void*)pmmngr_alloc_block();

	/* map user process stack space */
	vmmngr_mapPhysicalAddress(pProcess->pPageDirectory,
		(uint32_t)stack,
		(uint32_t)stackPhys,
		I86_PTE_PRESENT | I86_PTE_WRITABLE | I86_PTE_USER);

	/* Create Heap*/
	
	/* final initialization */
	pThread->initialStack = stack;
	pThread->frame.esp = (uint32_t)pThread->initialStack;
	pThread->frame.ebp = pThread->frame.esp;

	//DebugPrintf("\nThread Creation Success");
	
	return pThread;

}

Thread* ProcessManager::CreateMemoryThread(Process* pProcess, void(*lpStartAddress)())
{
	Thread* pThread = new Thread();
	pThread->pParent = pProcess;

	pThread->kernelStack = 0;
	pThread->priority = 1;
	pThread->state = PROCESS_STATE_ACTIVE;
	pThread->initialStack = 0;
	pThread->stackLimit = (void*)((uint32_t)pThread->initialStack + 4096);
	pThread->imageBase = 0;
	pThread->imageSize = 0;
	memset(&pThread->frame, 0, sizeof(trapFrame));
	pThread->frame.eip = (uint32_t)lpStartAddress;
	pThread->frame.flags = 0x200;

	/* Create userspace stack (process esp=0x100000) */
	void* stack = (void*)0x200000;
	void* stackPhys = (void*)pmmngr_alloc_block();

	/* map user process stack space */
	vmmngr_mapPhysicalAddress(pProcess->pPageDirectory,
		(uint32_t)stack,
		(uint32_t)stackPhys,
		I86_PTE_PRESENT | I86_PTE_WRITABLE);

	/* final initialization */
	pThread->initialStack = stack;
	pThread->frame.esp = (uint32_t)pThread->initialStack;
	pThread->frame.ebp = pThread->frame.esp;

	return pThread;
	
}

Process* ProcessManager::CreateProcess(char* appname)
{
	FILE file;

	/* open file */
	file = volOpenFile(appname);
	if (file.flags == FS_INVALID)
		return 0;
	if ((file.flags & FS_DIRECTORY) == FS_DIRECTORY)
		return 0;

	pdirectory* addressSpace = 0;

	addressSpace = vmmngr_createAddressSpace();
	if (!addressSpace) {		
		volCloseFile(&file);
		return 0;
	}

	mapKernelSpace(addressSpace);

	Process* pProcess = new Process();

	pProcess->TaskID = ProcessManager::GetInstance()->GetNextProcessId();
	pProcess->pPageDirectory = addressSpace;
	pProcess->dwPriority = 1;
	pProcess->dwRunState = PROCESS_STATE_ACTIVE;

	Thread* pThread = CreateThread(pProcess, &file);

	List_Add(&pProcess->pThreadQueue, "", pThread);	

	//ProcessManager::GetInstance()->g_pProcess = pProcess;
	//ProcessManager::GetInstance()->g_pThread = pThread;

	return pProcess;
}

Process* ProcessManager::SelectProcess()
{

	//if(g_pCurProcess)
		//DebugPrintf("\naaaaaa");

	return 0;
}

bool ProcessManager::ExecuteProcess(Process* pProcess)
{
	if (pProcess == 0)
		return false;

	int entryPoint = 0;
	unsigned int procStack = 0;

	if (pProcess->TaskID == PROC_INVALID_ID)
		return false;

	if (!pProcess->pPageDirectory)
		return false;

	Thread* pThread = (Thread*)List_GetData(pProcess->pThreadQueue, "", 0);
	//Thread* pThread = ProcessManager::GetInstance()->g_pThread;

	/* get esp and eip of main thread */
	entryPoint = pThread->frame.eip;
	procStack = pThread->frame.esp;

	DebugPrintf("\neip : %x", pThread->frame.eip);
	DebugPrintf("\npage directory : %x", pProcess->pPageDirectory);

	//__asm cli
	//List_Add(&ProcessManager::GetInstance()->pProcessQueue, "aaaaa", pProcess);
	ProcessManager::GetInstance()->g_pCurProcess = pProcess;
	//pmmngr_load_PDBR((physical_addr)pProcess->pPageDirectory);
	//__asm sti

	return true;
}
#include "../Include/Hal.h"
void StartRoutine()
{
	while (1) {
		_asm cli

		char* str = "\n\rHello world2!";

		DebugPrintf("\n%s", str);
		//PspSetupTaskSWEnv(); /* task-switching */
		outportb(0x20, 0x20); /* send EOI */

		_asm iretd
	}
	

	for (;;);
}

void Idle()
{
	


	for (;;);
}

void Idle2()
{



	for (;;);
}

extern BOOL HalWriteTssIntoGdt(TSS_32 *pTss32, DWORD TssSize, DWORD TssNumber, BOOL SetBusy);
extern BOOL HalSetupTaskLink(TSS_32 *pTss32, WORD TaskLink);
extern void install_tss(int startRoutine, uint32_t idx, uint16_t kernelSS, uint16_t kernelESP);
bool ProcessManager::CreateSystemProcess()
{
	Process* pProcess = new Process();
	pProcess->TaskID = 1;
	pProcess->pPageDirectory = vmmngr_createAddressSpace();
	mapKernelSpace(pProcess->pPageDirectory);

	pProcess->dwPriority = 1;
	pProcess->dwRunState = PROCESS_STATE_ACTIVE;

	Thread* pThread = CreateMemoryThread(pProcess, StartRoutine);

	//install_tss((int)StartRoutine, 5, 0x10, 0x9000);
	TSS_32* pTSSa = (TSS_32*)pmmngr_alloc_block();
	memset(pTSSa, 0, sizeof(TSS_32));
	HalSetupTSS(pTSSa, TRUE, (int)Idle2, (int*)pThread->frame.esp, (DWORD)pThread->stackLimit);
	HalSetupTaskLink(pTSSa, 0x28);
	HalWriteTssIntoGdt(pTSSa, sizeof(TSS_32), 0x30, FALSE);
	_asm {
		push	ax
			mov		ax, 0x30
			ltr		ax
			pop		ax
	}

	TSS_32* pTSS = (TSS_32*)pmmngr_alloc_block();
	memset(pTSS, 0, sizeof(TSS_32));
	HalSetupTSS(pTSS, TRUE, (int)Idle, (int*)pThread->frame.esp, (DWORD)pThread->stackLimit);
	HalWriteTssIntoGdt(pTSS, sizeof(TSS_32), 0x28, TRUE);
	

	List_Add(&pProcess->pThreadQueue, "", pThread);
	//List_Add(&pProcessQueue, "", pProcess);

	DebugPrintf("\nCreate Success System Process");

	return pProcess;
}