#include "sysapi.h"

void EntryPoint() {

	char* message ="\n\rHello world2!";
	__asm {

			mov eax, 4
			int 0x80
	}

	__asm {

		/* display message through kernel terminal */
		mov ebx, message
		mov eax, 0
		int 0x80
	}


	char* a = new char[100];
	strcpy(a, "\nProcess2 Reply.");

	__asm {

		/* display message through kernel terminal */
		mov ebx, a
			mov eax, 0
			int 0x80
	}

	int first = GetTickCount();
	while (1)
	{
		
		int second = GetTickCount();
		if (second - first > 100)
		{
			__asm
			{
				mov ebx, a	
				mov eax, 0
				int 0x80
			}

			first = GetTickCount();
		}

	}

	

	delete a;

	__asm {
			/* terminate */
			mov eax, 1
			int 0x80
	}
	for (;;);
}
