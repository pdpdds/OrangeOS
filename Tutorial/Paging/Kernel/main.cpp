#include "header.h"
#include "DebugDisplay.h"

int systemOn = 0;
int _cdecl kmain(multiboot_info* bootinfo) {

	DebugClrScr(0x13);
	DebugGotoXY(0, 0);
	DebugSetColor(0x17);

	uint32_t kernelSize = 0;
	_asm	mov	word ptr[kernelSize], dx
	
	DebugPrintf("\nExit command recieved; demo halted");
	_asm mov eax, 0xa0b0c0d0
	for (;;);
	return 0;
}

