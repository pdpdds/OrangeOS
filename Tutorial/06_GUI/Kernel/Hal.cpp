#include "Hal.h"
#include "pic.h"
#include "IDT.h"

void OutPortByte(ushort port, uchar value)
{
	_outp(port, value);
}

void OutPortWord(ushort port, ushort value)
{
	_outpw(port, value);
}

uchar InPortByte(ushort port)
{
	return (uchar)_inp(port);
}

ushort InPortWord(ushort port)
{
	return _inpw(port);
}

//! notifies hal interrupt is done
void InterruptDone(unsigned int intno) {

	//! insure its a valid hardware irq
	if (intno > 16)
		return;

	//! test if we need to send end-of-interrupt to second pic
	if (intno >= 8)
		i86_pic_send_command(I86_PIC_OCW2_MASK_EOI, 1);

	//! always send end-of-interrupt to primary pic
	i86_pic_send_command(I86_PIC_OCW2_MASK_EOI, 0);
}

//! sets new interrupt vector
void setvect(int intno, void(&vect) (), int flags) {

	//! install interrupt handler! This overwrites prev interrupt descriptor
	i86_install_ir(intno, (uint16_t)(I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | flags), 0x8, vect);
}

DWORD kReadFLAGS()
{
	UINT flag = 0;
	__asm
	{
		pushf; RFLAGS �������͸� ���ÿ� ����
		pop eax; ���ÿ� ����� RFLAGS �������͸� RAX �������Ϳ� �����Ͽ�		
		mov [flag], eax
	}

	return flag;
}

BOOL kSetInterruptFlag(BOOL bEnableInterrupt)
{
	DWORD dwRFLAGS;

	// ������ RFLAGS �������� ���� ���� �ڿ� ���ͷ�Ʈ ����/�Ұ� ó��
	dwRFLAGS = kReadFLAGS();
	if (bEnableInterrupt == TRUE)
	{
		InterruptEnable();
	}
	else
	{
		InterruptDisable();
	}

	// ���� RFLAGS ���������� IF ��Ʈ(��Ʈ 9)�� Ȯ���Ͽ� ������ ���ͷ�Ʈ ���¸� ��ȯ
	if (dwRFLAGS & 0x0200)
	{
		return TRUE;
	}
	return FALSE;
}