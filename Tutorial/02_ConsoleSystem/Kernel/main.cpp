#include "header.h"
#include "DebugDisplay.h"
#include "string.h"
#include "Console.h"

bool systemOn = false;
Console console;
extern uint32_t g_kernelSize;

int _cdecl kmain(multiboot_info* bootinfo) {

//커널사이즈에 512를 곱하면 바이트로 환산된다.
//현재 커널의 사이즈는 4096바이트다.
	g_kernelSize *= 512;
	
	console.SetBackColor(ConsoleColor::Blue);
	console.Clear();

	console.Print("Orange OS Console System Initialize\n");
	console.Print("KernelSize : %d Bytes\n", g_kernelSize);

	//헥사를 표시할 때 %X는 integer, %x는 unsigned integer의 헥사값을 표시한다.
	//주소값을 표기해야 하므로 %x를 사용한다.
	console.Print("systemOn Variable Address : 0x%x\n", &systemOn);

	char str[256];
	memset(str, 0, 256);

	return 0;
}