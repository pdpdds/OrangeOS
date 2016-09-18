#pragma once
#include "windef.h"

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// �����͸� �����ϴ� �ڷᱸ��
// �ݵ�� �������� ���� �պκп� ��ġ�ؾ� ��
typedef struct kListLinkStruct
{
    // ���� �������� ��巹���� �����͸� �����ϱ� ���� ID
    void* pvNext;
    DWORD qwID;
} LISTLINK;

/*
// ����Ʈ�� ����� �����͸� �����ϴ� ��
// �ݵ�� ���� �պκ��� LISTLINK�� �����ؾ� ��
struct kListItemExampleStruct
{
    // ����Ʈ�� �����ϴ� �ڷᱸ��
    LISTLINK stLink;
    
    // �����͵�
    int iData1;
    char cData2;
};
*/

// ����Ʈ�� �����ϴ� �ڷᱸ��
typedef struct kListManagerStruct
{
    // ����Ʈ �������� ��
    int iItemCount;

    // ����Ʈ�� ù ��°�� ������ �������� ��巹��
    void* pvHeader;
    void* pvTail;
} LIST;

#pragma pack( pop )


////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
void kInitializeList( LIST* pstList );
int kGetListCount( const LIST* pstList );
void kAddListToTail( LIST* pstList, void* pvItem );
void kAddListToHeader( LIST* pstList, void* pvItem );
void* kRemoveList( LIST* pstList, DWORD qwID );
void* kRemoveListFromHeader( LIST* pstList );
void* kRemoveListFromTail( LIST* pstList );
void* kFindList( const LIST* pstList, DWORD qwID );
void* kGetHeaderFromList( const LIST* pstList );
void* kGetTailFromList( const LIST* pstList );
void* kGetNextFromList( const LIST* pstList, void* pstCurrent );
