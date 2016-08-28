#include "header.h"
#include "DebugDisplay.h"
#include "string.h"
#include "Console.h"

bool systemOn = false;
Console console;
extern uint32_t g_kernelSize;

int _cdecl kmain(multiboot_info* bootinfo) {

//Ŀ�λ���� 512�� ���ϸ� ����Ʈ�� ȯ��ȴ�.
//���� Ŀ���� ������� 4096����Ʈ��.
	g_kernelSize *= 512;
	
	console.SetBackColor(ConsoleColor::Blue);
	console.Clear();

	console.Print("Orange OS Console System Initialize\n");
	console.Print("KernelSize : %d Bytes\n", g_kernelSize);

	//��縦 ǥ���� �� %X�� integer, %x�� unsigned integer�� ��簪�� ǥ���Ѵ�.
	//�ּҰ��� ǥ���ؾ� �ϹǷ� %x�� ����Ѵ�.
	console.Print("systemOn Variable Address : 0x%x\n", &systemOn);

	char str[256];
	memset(str, 0, 256);

	return 0;
}