/**
 *  file    Mouse.c
 *  date    2009/09/26
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ���콺 ����̽� ����̹��� ���õ� �ҽ� ����
 */

#include "Mouse.h"
#include "Keyboard.h"
#include "Queue.h"
#include "Hal.h"

// ���콺 ���¸� �����ϴ� ���콺 �Ŵ���
static MOUSEMANAGER gs_stMouseManager = { 0, };

// ���콺�� �����ϴ� ť�� ���� ����
static QUEUE gs_stMouseQueue;
static MOUSEDATA gs_vstMouseQueueBuffer[ MOUSE_MAXQUEUECOUNT ];

/**
 *  ���콺 �ʱ�ȭ
 */
BOOL kInitializeMouse( void )
{
    // ť �ʱ�ȭ
    kInitializeQueue( &gs_stMouseQueue, gs_vstMouseQueueBuffer, MOUSE_MAXQUEUECOUNT, 
            sizeof( MOUSEDATA ) );
    
    // ���ɶ� �ʱ�ȭ
    kInitializeSpinLock( &( gs_stMouseManager.stSpinLock ) );
    
    // ���콺 Ȱ��ȭ
    if( kActivateMouse() == TRUE )
    {
        // ���콺 ���ͷ�Ʈ Ȱ��ȭ
        kEnableMouseInterrupt();
        return TRUE;
    }
    return FALSE;
}

/**
 *  ���콺�� Ȱ��ȭ ��
 */
BOOL kActivateMouse( void )
{
    int i = 0;
    BOOL bPreviousInterrupt;
    BOOL bResult;
    
    // ���ͷ�Ʈ �Ұ�
    bPreviousInterrupt = kSetInterruptFlag( FALSE );
    
    // ��Ʈ�� ��������(��Ʈ 0x64)�� ���콺 Ȱ��ȭ Ŀ�ǵ�(0xA8)�� �����Ͽ� ���콺 ����̽� Ȱ��ȭ
    OutPortByte( 0x64, 0xA8 );
   
    // ��Ʈ�� ��������(��Ʈ 0x64)�� ���콺�� �����͸� �����ϴ� Ŀ�ǵ�(0xD4)�� �����Ͽ� 
    // �Է� ����(��Ʈ 0x60)�� ���޵� �����͸� ���콺�� ����
    OutPortByte( 0x64, 0xD4 );
    
    // �Է� ����(��Ʈ 0x60)�� �� ������ ��ٷȴٰ� ���콺�� Ȱ��ȭ Ŀ�ǵ带 ����
    // 0xFFFF��ŭ ������ ������ �ð��̸� ����� Ŀ�ǵ尡 ���۵� �� ����
    // 0xFFFF ������ ������ ���Ŀ��� �Է� ����(��Ʈ 0x60)�� ���� ������ �����ϰ� ����
    for( i = 0 ; i < 0xFFFF ; i++ )
    {
        // �Է� ����(��Ʈ 0x60)�� ��������� Ű���� Ŀ�ǵ� ���� ����
        if( kIsInputBufferFull() == FALSE )
        {
            break;
        }
    }
    
    // �Է� ����(��Ʈ 0x60)�� ���콺 Ȱ��ȭ(0xF4) Ŀ�ǵ带 �����Ͽ� ���콺�� ����
    OutPortByte( 0x60, 0xF4 );
    
    // ACK�� �� ������ �����
    bResult = kWaitForACKAndPutOtherScanCode();
    
    // ���� ���ͷ�Ʈ ���� ����
    kSetInterruptFlag( bPreviousInterrupt );
    return bResult;
}

/**
 *  ���콺 ���ͷ�Ʈ�� Ȱ��ȭ
 */
void kEnableMouseInterrupt( void )
{
    BYTE bOutputPortData;
    int i;
    
    // Ŀ�ǵ� ����Ʈ �б�
    // ��Ʈ�� ��������(��Ʈ 0x64)�� Ű���� ��Ʈ�ѷ��� Ŀ�ǵ� ����Ʈ�� �д� Ŀ�ǵ�(0x20) ����
    OutPortByte( 0x64, 0x20 );
    
    // ��� ��Ʈ�� �����͸� ��ٷȴٰ� ����
    for( i = 0 ; i < 0xFFFF ; i++ )
    {
        // ��� ����(��Ʈ 0x60)�� �������� �����͸� ���� �� ���� 
        if( kIsOutputBufferFull() == TRUE )
        {
            break;
        }
    }
    // ��� ��Ʈ(��Ʈ 0x60)�� ���ŵ� Ŀ�ǵ� ����Ʈ ���� ����
    bOutputPortData = InPortByte( 0x60 );
    

    // ���콺 ���ͷ�Ʈ ��Ʈ Ȱ��ȭ�� �� Ŀ�ǵ� ����Ʈ ����
    // ���콺 ���ͷ�Ʈ ��Ʈ(��Ʈ 1) ����
    bOutputPortData |= 0x02;

    // Ŀ�ǵ� ��������(0x64)�� Ŀ�ǵ� ����Ʈ�� ���� Ŀ�ǵ�(0x60)�� ����
    OutPortByte( 0x64, 0x60 );

    // �Է� ����(��Ʈ 0x60)�� �����Ͱ� ��������� ��� ��Ʈ�� ���� ���� Ŀ�ǵ�� Ŀ�ǵ� ����Ʈ ����
    for( i = 0 ; i < 0xFFFF ; i++ )
    {
        // �Է� ����(��Ʈ 0x60)�� ������� Ŀ�ǵ� ���� ����
        if( kIsInputBufferFull() == FALSE )
        {
            break;
        }
    }
    
    // �Է� ����(0x60)�� ���콺 ���ͷ�Ʈ ��Ʈ�� 1�� ������ ���� ����
    OutPortByte( 0x60, bOutputPortData );
}


/**
 *  ���콺 �����͸� ��Ƽ� ť�� ����
 */
BOOL kAccumulateMouseDataAndPutQueue( BYTE bMouseData )
{
    BOOL bResult = FALSE;
    
    // ���ŵ� ����Ʈ ���� ���� ���콺 �����͸� ����
    switch( gs_stMouseManager.iByteCount )
    {
        // ����Ʈ 1�� ������ ����
    case 0:
        gs_stMouseManager.stCurrentData.bButtonStatusAndFlag = bMouseData;
        gs_stMouseManager.iByteCount++;
        break;
        
        // ����Ʈ 2�� ������ ����
    case 1:
        gs_stMouseManager.stCurrentData.bXMovement = bMouseData;
        gs_stMouseManager.iByteCount++;
        break;
        
        // ����Ʈ 3�� ������ ����
    case 2:
        gs_stMouseManager.stCurrentData.bYMovement = bMouseData;
        gs_stMouseManager.iByteCount++;
        break;
        
        // �� ���� ���� ���ŵ� ����Ʈ �� �ʱ�ȭ
    default:
        gs_stMouseManager.iByteCount = 0;
        break;
    }
    
    // 3����Ʈ�� ��� ���ŵǾ����� ���콺 ť�� �����ϰ� ���ŵ� Ƚ���� �ʱ�ȭ
    if( gs_stMouseManager.iByteCount >= 3 )
    {
        // �Ӱ� ���� ����
        kLockForSpinLock( &( gs_stMouseManager.stSpinLock ) );
        
        // ���콺 ť�� ���콺 ������ ���� 
        bResult = kPutQueue( &gs_stMouseQueue, &gs_stMouseManager.stCurrentData );    
        // �Ӱ� ���� ��
        kUnlockForSpinLock( &( gs_stMouseManager.stSpinLock ) );
        // ���ŵ� ����Ʈ �� �ʱ�ȭ
        gs_stMouseManager.iByteCount = 0;
    }
    return bResult;
}

/**
 *  ���콺 ť���� ���콺 �����͸� ����
 */
BOOL kGetMouseDataFromMouseQueue( BYTE* pbButtonStatus, int* piRelativeX, 
        int* piRelativeY )
{
    MOUSEDATA stData;
    BOOL bResult;

    // ť�� ��������� �����͸� ���� �� ����
    if( kIsQueueEmpty( &( gs_stMouseQueue ) ) == TRUE )
    {
        return FALSE;
    }
    
    // �Ӱ� ���� ����
    kLockForSpinLock( &( gs_stMouseManager.stSpinLock ) );
    // ť���� �����͸� ����
    bResult = kGetQueue( &( gs_stMouseQueue ), &stData );    
    // �Ӱ� ���� ��
    kUnlockForSpinLock( &( gs_stMouseManager.stSpinLock ) );
    
    // �����͸� ������ �������� ����
    if( bResult == FALSE )
    {
        return FALSE;
    }
    
    // ���콺 ������ �м�
    // ���콺 ��ư �÷��״� ù ��° ����Ʈ�� ���� 3��Ʈ�� ������
    *pbButtonStatus = stData.bButtonStatusAndFlag & 0x7;

    // X, Y�� �̵��Ÿ� ����
    // X�� ��ȣ ��Ʈ�� ��Ʈ 4�� ������ 1�� �����Ǿ������� ������
    *piRelativeX = stData.bXMovement & 0xFF;
    if( stData.bButtonStatusAndFlag & 0x10 )
    {
        // �����̹Ƿ� �Ʒ� 8��Ʈ�� X �̵��Ÿ��� ������ �� ���� ��Ʈ�� ��� 1�� �����
        // ��ȣ ��Ʈ�� Ȯ���� 
        *piRelativeX |= ( 0xFFFFFF00 );
    }
    
    // Y�� ��ȣ ��Ʈ�� ��Ʈ 5�� ������, 1�� �����Ǿ����� ������
    // �Ʒ� �������� ������ Y ���� �����ϴ� ȭ�� ��ǥ�� �޸� ���콺�� ���� �������� ������
    // ���� �����ϹǷ� ����� ���� �� ��ȣ�� ������
    *piRelativeY = stData.bYMovement & 0xFF;
    if( stData.bButtonStatusAndFlag & 0x20 )
    {
        // �����̹Ƿ� �Ʒ� 8��Ʈ�� Y �̵��Ÿ��� ������ �� ���� ��Ʈ�� ��� 1�� �����
        // ��ȣ ��Ʈ�� Ȯ���� 
        *piRelativeY |= ( 0xFFFFFF00 );
    }

    // ���콺�� Y�� ���� ������ ȭ�� ��ǥ�� �ݴ��̹Ƿ� Y �̵��Ÿ��� -�Ͽ� ������ �ٲ�
    *piRelativeY = -*piRelativeY;
    return TRUE;
}

/**
 *  ���콺 �����Ͱ� ��� ���ۿ� �ִ����� ��ȯ
 */
BOOL kIsMouseDataInOutputBuffer( void )
{
    // ��� ����(��Ʈ 0x60�� �б� ���� ���� ���� ��������(��Ʈ 0x64)�� �о
    // ���콺 �������ΰ��� Ȯ��, ���콺 �����ʹ� AUXB ��Ʈ(��Ʈ 5)�� 1�� ������
    if( InPortByte( 0x64 ) & 0x20 )
    {
        return TRUE;
    }
    return FALSE;
}
