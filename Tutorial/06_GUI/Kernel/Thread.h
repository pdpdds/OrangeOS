#pragma once
#include "windef.h"
#include "task.h"
#include "Hal.h"

#define TASK_RUNNING            1
#define TASK_RUNNABLE           2
#define TASK_INTERRUPTIBLE      4
//#define TASK_UNINTERRUPTIBLE    
#define TASK_STOPPED            8

#define DEFAULT_THREAD_MESSAGES 100

enum STACK_POS
{
	SP_P_SS = 1,
	SP_P_ESP,
	SP_EFLAGS,
	SP_CS,
	SP_EIP,
	SP_EAX,
	SP_ECX,
	SP_EDX,
	SP_EBX,
	SP_ESP,
	SP_EBP,
	SP_ESI,
	SP_EDI,
	SP_DS,
	SP_ES,
	SP_FS,
	SP_GS,
};

#define STACK_MIN_SIZE SP_GS

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

struct ThreadStackInfo
{
	UINT32 SS0, SS1, SS2, SS3;
	UINT32 ESP0, ESP1, ESP2, ESP3;
};

#ifdef _MSC_VER
#pragma pack (pop, 1)
#endif

class Process;

class Thread
{
public:
	Thread();
	virtual ~Thread();

	BYTE * IOBASE;

	UINT32 * lpStack;

//Thread Local Storage
	void* lpTLS = NULL;

	UINT32 dwRunState;

	UINT32 dwPriority;
	int m_waitingTime;
	ThreadStackInfo StackInfo;

	Process* m_pParent;


	LPVOID	startParam;
	void*     initialStack; /* virtual address */
	void*     stackLimit;
	void*     kernelStack;
	uint32_t  priority;
	int       state;
	trapFrame frame;
	uint32_t  imageBase;
	uint32_t  imageSize;	

	uint32_t curESP;
	
	registers_t m_regs;
};