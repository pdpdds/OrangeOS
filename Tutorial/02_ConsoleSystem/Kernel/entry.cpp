/******************************************************************************
   entry.cpp
		-Kernel entry point

   modified\ Aug 06 2008
   arthor\ Mike
******************************************************************************/

#include <bootinfo.h>
#include "DebugDisplay.h"
#include "Console.h"

//! basic crt initilization stuff
extern void __cdecl  InitializeConstructors ();
extern void __cdecl  Exit ();
extern Console console;
uint32_t g_kernelSize = 0;

//! main
extern int _cdecl kmain (multiboot_info* bootinfo);

//! kernel entry point is called by boot loader
void __cdecl  kernel_entry (multiboot_info* bootinfo) {

#ifdef ARCH_X86

	// Set registers for protected mode
	_asm {
		cli
		mov ax, 10h
		mov ds, ax
		mov es, ax
		mov fs, ax
		mov gs, ax
	}
#endif //ARCH_X86

	//dx 레지스터에는 커널의 크기가 담겨 있다.
	//다른값으로 씌워지기 전에 값을 얻어낸다.
	_asm	mov	word ptr[g_kernelSize], dx
	InitializeConstructors();
	kmain (bootinfo);
	Exit ();

	console.Print("kernel_entry : Shutdown Complete. Halting system\n");

#ifdef ARCH_X86
	_asm {
		cli
		hlt
	}
#endif

	for (;;);
}
