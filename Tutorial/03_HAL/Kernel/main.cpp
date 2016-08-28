#include "header.h"
#include "DebugDisplay.h"
#include "string.h"
#include "sprintf.h"
#include "Console.h"
#include "PIC.h"
#include "PIT.h"
#include "HAL.h"
#include "RTC.H"
#include "IDT.h"
#include "GDT.h"

bool systemOn = false;
uint32_t kernelSize = 0;
Console console;

void PrintSystemInfo();

int _cdecl kmain(multiboot_info* bootinfo) {

//dx �������Ϳ��� Ŀ���� ũ�Ⱑ ��� �ִ�.
//�ٸ������� �������� ���� ���� ����.
	_asm	mov	word ptr[kernelSize], dx

	InterruptDisable();

	i86_gdt_initialize();
	i86_idt_initialize(0x8);
	i86_pic_initialize(0x20, 0x28);
	i86_pit_initialize();
	i86_pit_start_counter(100, I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);

	InterruptEnable();

	PrintSystemInfo();

	char szBuf[256];
	memset(szBuf, 0, 256);

	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	sprintf(szBuf, "LocalTime : %d %d/%d %d:%d %d\n", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	console.Print("%s", szBuf);

	sprintf(szBuf, "Address : %x\n", (unsigned int)&kernelSize);

	console.Print("%s", szBuf);

	return 0;
}

void PrintSystemInfo()
{
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
}