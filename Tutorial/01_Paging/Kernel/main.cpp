#include "header.h"
#include "DebugDisplay.h"
#include "string.h"

bool systemOn = false;

int _cdecl kmain(multiboot_info* bootinfo) {

//dx 레지스터에는 커널의 크기가 담겨 있다
//다른값으로 씌워지기 전에 값을 얻어낸다
	uint32_t kernelSize = 0;
	_asm	mov	word ptr[kernelSize], dx

//커널사이즈에 512를 곱하면 바이트로 환산된다
//현재 커널의 사이즈는 4096바이트다.
	kernelSize *= 512;

	DebugClrScr(0x13);
	DebugGotoXY(0, 0);
	DebugSetColor(0x17);

	char name[256];
	memset(name, 0, 256);
	
	DebugPrintf("\nKernelSize : %d Bytes", kernelSize);
	DebugPrintf("\nsystemOn Variable Address : 0x%x", &systemOn);
	return 0;
}