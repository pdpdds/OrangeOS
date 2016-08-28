#include "header.h"
#include "DebugDisplay.h"
#include "string.h"
#include "Console.h"

bool systemOn = false;
Console console;

int _cdecl kmain(multiboot_info* bootinfo) {

//dx �������Ϳ��� Ŀ���� ũ�Ⱑ ��� �ִ�.
//�ٸ������� �������� ���� ���� ����.
	uint32_t kernelSize = 0;
	_asm	mov	word ptr[kernelSize], dx

//Ŀ�λ���� 512�� ���ϸ� ����Ʈ�� ȯ��ȴ�.
//���� Ŀ���� ������� 4096����Ʈ��.
	kernelSize *= 512;
	
	console.SetBackColor(ConsoleColor::Blue);
	console.Clear();

	console.Print("Orange OS Console System Initialize\n");
	console.Print("KernelSize : %d Bytes\n", kernelSize);

	//��縦 ǥ���� �� %X�� integer, %x�� unsigned integer�� ��簪�� ǥ���Ѵ�.
	//�ּҰ��� ǥ���ؾ� �ϹǷ� %x�� ����Ѵ�.
	console.Print("systemOn Variable Address : 0x%x\n", &systemOn);

	return 0;
}