/**
 *  file    Synchronization.c
 *  date    2009/03/13
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   ����ȭ�� ó���ϴ� �Լ��� ���õ� ����
 */

#include "Synchronization.h"
#include "kTask.h"
#include "Hal.h"

// ���ɶ� ���� ������ ����ϴ� �Լ���
#if 0
/**
 *  �ý��� �������� ����ϴ� �����͸� ���� ��� �Լ�
 */
BOOL kLockForSystemData( void )
{
    return kSetInterruptFlag( FALSE );
}

/**
 *  �ý��� �������� ����ϴ� �����͸� ���� ��� ���� �Լ�
 */
void kUnlockForSystemData( BOOL bInterruptFlag )
{
    kSetInterruptFlag( bInterruptFlag );
}
#endif

/**
 *  ���ؽ��� �ʱ�ȭ 
 */
void kInitializeMutex( MUTEX* pstMutex )
{
    // ��� �÷��׿� Ƚ��, �׸��� �½�ũ ID�� �ʱ�ȭ
    pstMutex->bLockFlag = FALSE;
    pstMutex->dwLockCount = 0;
    pstMutex->dwTaskID = TASK_INVALIDID;
}

BYTE kGetAPICID()
{
	return 0;
}

/**
 *  �½�ũ ���̿��� ����ϴ� �����͸� ���� ��� �Լ�
 */
void kLock( MUTEX* pstMutex )
{
    /*BYTE bCurrentAPICID;
    BOOL bInterruptFlag;

    // ���ͷ�Ʈ�� ��Ȱ��ȭ
    bInterruptFlag = kSetInterruptFlag( FALSE );

    // ���� �ھ��� ���� APIC ID�� Ȯ��
    bCurrentAPICID = kGetAPICID();
    
    // �̹� ��� �ִٸ� ���� �ᰬ���� Ȯ���ϰ� ��� Ƚ���� ������Ų �� ����
    if( kTestAndSet(&( pstMutex->bLockFlag ), 0, 1 ) == FALSE )
    {
        // �ڽ��� �ᰬ�ٸ� Ƚ���� ������Ŵ
        if( pstMutex->dwTaskID == kGetRunningTask( bCurrentAPICID )->stLink.qwID ) 
        {
            // ���ͷ�Ʈ�� ����
            kSetInterruptFlag( bInterruptFlag );
            pstMutex->dwLockCount++;
            return ;
        }
        
        // �ڽ��� �ƴ� ���� ��� ���� ������ ������ ���
        while( kTestAndSet( &( pstMutex->bLockFlag ), 0, 1 ) == FALSE )
        {
            kSchedule();
        }
    }
       
    // ��� ����, ��� �÷��״� ���� kTestAndSet() �Լ����� ó����
    pstMutex->dwLockCount = 1;
    pstMutex->dwTaskID = kGetRunningTask( bCurrentAPICID )->stLink.qwID;
    // ���ͷ�Ʈ�� ����
    kSetInterruptFlag( bInterruptFlag );*/
}

/**
 *  �½�ũ ���̿��� ����ϴ� �����͸� ���� ��� ���� �Լ�
 */
void kUnlock( MUTEX* pstMutex )
{
   /* BOOL bInterruptFlag;

    // ���ͷ�Ʈ�� ��Ȱ��ȭ
    bInterruptFlag = kSetInterruptFlag( FALSE );
    
    // ���ؽ��� ��� �½�ũ�� �ƴϸ� ����
    if( ( pstMutex->bLockFlag == FALSE ) ||
        ( pstMutex->dwTaskID != kGetRunningTask( kGetAPICID() )->stLink.qwID ) )
    {
        // ���ͷ�Ʈ�� ����
        kSetInterruptFlag( bInterruptFlag );
        return ;
    }
    
    // ���ؽ��� �ߺ����� �ᰬ���� ��� Ƚ���� ����
    if( pstMutex->dwLockCount > 1 )
    {
        pstMutex->dwLockCount--;
    }
    else
    {
        // ������ ������ ����, ��� �÷��׸� ���� ���߿� �����ؾ� ��
        pstMutex->dwTaskID = TASK_INVALIDID;
        pstMutex->dwLockCount = 0;
        pstMutex->bLockFlag = FALSE;
    }
    // ���ͷ�Ʈ�� ����
    kSetInterruptFlag( bInterruptFlag );*/
}

/**
 *  ���ɶ��� �ʱ�ȭ
 */
void kInitializeSpinLock( SPINLOCK* pstSpinLock )
{
    // ��� �÷��׿� Ƚ��, APIC ID, ���ͷ�Ʈ �÷��׸� �ʱ�ȭ
    pstSpinLock->bLockFlag = FALSE;
    pstSpinLock->dwLockCount = 0;
    pstSpinLock->bAPICID = 0xFF;
    pstSpinLock->bInterruptFlag = FALSE;
}

/**
 *  �ý��� �������� ����ϴ� �����͸� ���� ��� �Լ�
 */
void kLockForSpinLock( SPINLOCK* pstSpinLock )
{
   /* BOOL bInterruptFlag;
    
    // ���ͷ�Ʈ�� ���� ��Ȱ��ȭ
    bInterruptFlag = kSetInterruptFlag( FALSE );

    // �̹� ��� �ִٸ� ���� �ᰬ���� Ȯ���ϰ� �׷��ٸ� ��� Ƚ���� ������Ų �� ����
    if( kTestAndSet(&( pstSpinLock->bLockFlag ), 0, 1 ) == FALSE )
    {
        // �ڽ��� �ᰬ�ٸ� Ƚ���� ������Ŵ
        if( pstSpinLock->bAPICID == kGetAPICID() )
        {
            pstSpinLock->dwLockCount++;
            return ;
        }
        
        // �ڽ��� �ƴ� ���� ��� ���� ������ ������ ���
        while( kTestAndSet( &( pstSpinLock->bLockFlag ), 0, 1 ) == FALSE )
        {
            // kTestAndSet() �Լ��� ��� ȣ���Ͽ� �޸� ������ Lock �Ǵ� ���� ����
            while( pstSpinLock->bLockFlag == TRUE )
            {
				__asm
				{
					pause
				}
            }
        }
    }
    
    // ��� ����, ��� �÷��״� ���� kTestAndSet() �Լ����� ó����
    pstSpinLock->dwLockCount = 1;
    pstSpinLock->bAPICID = kGetAPICID();

    // ���ͷ�Ʈ �÷��׸� �����Ͽ� Unlock ���� �� ����
    pstSpinLock->bInterruptFlag = bInterruptFlag;*/
}

/**
 *  �ý��� �������� ����ϴ� �����͸� ���� ��� ���� �Լ�
 */
void kUnlockForSpinLock( SPINLOCK* pstSpinLock )
{
    /*BOOL bInterruptFlag;
    
    // ���ͷ�Ʈ�� ���� ��Ȱ��ȭ
    bInterruptFlag = kSetInterruptFlag( FALSE );

    // ���ɶ��� ��� �½�ũ�� �ƴϸ� ����
    if( ( pstSpinLock->bLockFlag == FALSE ) ||
        ( pstSpinLock->bAPICID != kGetAPICID() ) )
    {
        kSetInterruptFlag( bInterruptFlag );
        return ;
    }
    
    // ���ɶ��� �ߺ����� �ᰬ���� ��� Ƚ���� ����
    if( pstSpinLock->dwLockCount > 1 )
    {
        pstSpinLock->dwLockCount--;
        return ;
    }
    
    // ���ɶ��� ������ ������ �����ϰ� ���ͷ�Ʈ �÷��׸� ����
    // ���ͷ�Ʈ �÷��״� �̸� �����صξ��ٰ� ���
    bInterruptFlag = pstSpinLock->bInterruptFlag;    
    pstSpinLock->bAPICID = 0xFF;
    pstSpinLock->dwLockCount = 0;
    pstSpinLock->bInterruptFlag = FALSE;
    pstSpinLock->bLockFlag = FALSE;  
    
    kSetInterruptFlag( bInterruptFlag );*/
}
