#include "header.h"
#include "DebugDisplay.h"
#include "string.h"
#include "Console.h"

bool systemOn = false;
Console console;

int _cdecl kmain(multiboot_info* bootinfo) {

//dx 레지스터에는 커널의 크기가 담겨 있다.
//다른값으로 씌워지기 전에 값을 얻어낸다.
	uint32_t kernelSize = 0;
	_asm	mov	word ptr[kernelSize], dx

//커널사이즈에 512를 곱하면 바이트로 환산된다.
//현재 커널의 사이즈는 4096바이트다.
	kernelSize *= 512;
	
	console.SetBackColor(ConsoleColor::Blue);
	console.Clear();

	console.Print("Orange OS Console System Initialize\n");
	console.Print("KernelSize : %d Bytes\n", kernelSize);

	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.
	//주소값을 표기해야 하므로 %x를 사용한다.
	console.Print("systemOn Variable Address : 0x%x\n", &systemOn);

	return 0;
}