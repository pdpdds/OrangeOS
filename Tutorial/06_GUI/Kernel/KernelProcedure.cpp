#include "KernelProcedure.h"
#include "ConsoleManager.h"
#include "Console.h"
#include "string.h"
#include "Hal.h"
#include "PIT.h"
#include "Process.h"

bool systemOn = false;
extern Console console;
extern "C" uint32_t GetSysytemTickCount();

void NativeConsole()
{
	ConsoleManager manager;

	char	commandBuffer[MAXPATH];

	while (1)
	{
		console.Print("Command> ");
		memset(commandBuffer, 0, MAXPATH);
		//console.Print("commandBuffer Address : 0x%x\n", &commandBuffer);	

		console.GetCommand(commandBuffer, MAXPATH - 2);
		console.Print("\n");

		if (manager.RunCommand(commandBuffer) == true)
			break;

		/*int first = GetTickCount();
		int count = 4;
		while (count != 0)
		{

		int second = GetTickCount();
		if (second - first > 100)
		{
		console.Print("%d\n", second);

		first = GetTickCount();
		}
		}*/
	}
}

DWORD WINAPI SystemEntry(LPVOID parameter)
{	
	while (1) {
		NativeConsole();
	}

	for (;;);

	return 0;
}

DWORD WINAPI SampleLoop(LPVOID parameter)
{
	char* str = "Sample Loop!!\n";
	console.Print("%s", str);
	int first = GetSysytemTickCount();
	bool bExit = false;
	console.Print("%s", str);	

	Process* pProcess = (Process*)parameter;
	console.Print("Thread Routine Process Address %x\n", pProcess);

	while (bExit == false)
	{
		static int count = 0;
		int second = GetSysytemTickCount();
		if (second - first > 100)
		{
			console.Print("%s", str);

			first = GetSysytemTickCount();
			count++;
		}
	}

	return 0;
}

DWORD WINAPI TaskProcessor(LPVOID parameter)
{
	Process* pProcess = (Process*)parameter;
	console.Print("TaskProcessor Thread Parent Process Address %x\n", pProcess);

	while (true)
	{
	//	simpl
	}

	return 0;
}

DWORD WINAPI TestKernelProcess(LPVOID parameter)
{		
	Process* pProcess = (Process*)parameter;
	console.Print("Test Second Kernel Process %x\n", pProcess);
	BOOL bExit = false;
	int first = GetSysytemTickCount();

	while (bExit == false)
	{
		static int count = 0;
		
		int second = GetSysytemTickCount();
		if (second - first > 100)
		{
			console.Print("Test Second Kernel Process %x\n", pProcess);

			first = GetSysytemTickCount();
			count++;
		}
	}

	return 0;
}
