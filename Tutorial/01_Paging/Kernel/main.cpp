#include "header.h"
#include "DebugDisplay.h"
#include "string.h"

bool systemOn = false;

int _cdecl kmain(multiboot_info* bootinfo) {

//dx �������Ϳ��� Ŀ���� ũ�Ⱑ ��� �ִ�
//�ٸ������� �������� ���� ���� ����
	uint32_t kernelSize = 0;
	_asm	mov	word ptr[kernelSize], dx

//Ŀ�λ���� 512�� ���ϸ� ����Ʈ�� ȯ��ȴ�
//���� Ŀ���� ������� 4096����Ʈ��.
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