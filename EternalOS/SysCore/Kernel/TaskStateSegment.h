#pragma once
#include "windef.h"

#ifdef _MSC_VER
#pragma pack (push, 1)
#endif

/////////////////////////////////////////////////////////
//������ ���ؽ�Ʈ ����Ī�� �����ؾ� �� �����͵�. �ش� �����尡 ������ �� �������Ϳ� ������ �����ؾ� �Ѵ�.
//CR3 ����¡�� ���� ������ ���丮�� �ּҸ� ����Ŵ
//EIP Instruction Pointer�� �ּ�
/////////////////////////////////////////////////////////

struct TaskStateSegment
{
	UINT32 PREV_LINK;
	UINT32 ESP0;
	UINT32 SS0;
	UINT32 ESP1;
	UINT32 SS1;
	UINT32 ESP2;
	UINT32 SS2;
	UINT32 CR3;
	UINT32 EIP;
	UINT32 EFLAGS;
	UINT32 EAX;
	UINT32 ECX;
	UINT32 EDX;
	UINT32 EBX;
	UINT32 ESP;
	UINT32 EBP;
	UINT32 ESI;
	UINT32 EDI;
	UINT32 ES;
	UINT32 CS;
	UINT32 SS;
	UINT32 DS;
	UINT32 FS;
	UINT32 GS;
	UINT32 LDT_SEL;
	UINT16 TRAP;
	UINT16 IOBASE;
};

#ifdef _MSC_VER
#pragma pack (pop, 1)
#endif