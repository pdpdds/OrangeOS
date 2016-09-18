/**
 *  file    Synchronization.h
 *  date    2009/03/13
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   ����ȭ�� ó���ϴ� �Լ��� ���õ� ����
 */

#ifndef __SYNCHRONIZATION_H__
#define __SYNCHRONIZATION_H__

#include "windef.h"

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// ���ؽ� �ڷᱸ��
typedef struct kMutexStruct
{
    // �½�ũ ID�� ����� ������ Ƚ��
    volatile DWORD dwTaskID;
    volatile DWORD dwLockCount;

    // ��� �÷���
    volatile BOOL bLockFlag;
    
    // �ڷᱸ���� ũ�⸦ 8����Ʈ ������ ���߷��� �߰��� �ʵ�
    BYTE vbPadding[ 3 ];
} MUTEX;

// ���ɶ� �ڷᱸ��
typedef struct kSpinLockStruct
{
    // ���� APIC ID�� ����� ������ Ƚ��
    volatile DWORD dwLockCount;
    volatile BYTE bAPICID;

    // ��� �÷���
    volatile BOOL bLockFlag;
    
    // ���ͷ�Ʈ �÷���
    volatile BOOL bInterruptFlag;
    
    // �ڷᱸ���� ũ�⸦ 8����Ʈ ������ ���߷��� �߰��� �ʵ�
    BYTE vbPadding[ 1 ];
} SPINLOCK;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
#if 0
BOOL kLockForSystemData( void );
void kUnlockForSystemData( BOOL bInterruptFlag );
#endif

void kInitializeSpinLock( SPINLOCK* pstSpinLock );
void kLockForSpinLock( SPINLOCK* pstSpinLock );
void kUnlockForSpinLock( SPINLOCK* pstSpinLock );

void kInitializeMutex( MUTEX* pstMutex );
void kLock( MUTEX* pstMutex );
void kUnlock( MUTEX* pstMutex );

#endif /*__SYNCHRONIZATION_H__*/
