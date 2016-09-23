#include "KernelProcedure.h"
#include "ConsoleManager.h"
#include "Console.h"
#include "string.h"

extern Console console;

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