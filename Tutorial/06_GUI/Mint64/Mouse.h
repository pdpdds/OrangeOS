/**
 *  file    Mouse.h
 *  date    2009/09/26
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   ���콺 ����̽� ����̹��� ���õ� ��� ����
 */

#ifndef __MOUSE_H__
#define __MOUSE_H__

#include "windef.h"
#include "Synchronization.h"

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// ���콺 ť�� ���� ��ũ��
#define MOUSE_MAXQUEUECOUNT 100

// ��ư�� ���¸� ��Ÿ���� ��ũ��
#define MOUSE_LBUTTONDOWN   0x01
#define MOUSE_RBUTTONDOWN   0x02
#define MOUSE_MBUTTONDOWN   0x04

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// PS/2 ���콺 ��Ŷ�� �����ϴ� �ڷᱸ��, ���콺 ť�� �����ϴ� ������
typedef struct kMousePacketStruct
{
    // ��ư ���¿� X, Y ���� ���õ� �÷���
    BYTE bButtonStatusAndFlag;    
    // X�� �̵��Ÿ�
    BYTE bXMovement;    
    // Y�� �̵��Ÿ�
    BYTE bYMovement;
} MOUSEDATA;

#pragma pack( pop )

// ���콺�� ���¸� �����ϴ� �ڷᱸ��
typedef struct kMouseManagerStruct
{
    // �ڷᱸ�� ����ȭ�� ���� ���ɶ�
    SPINLOCK stSpinLock;    
    // ���� ���ŵ� �������� ����, ���콺 �����Ͱ� 3���̹Ƿ� 0~2�� ������ ��� �ݺ���
    int iByteCount;
    // ���� ���� ���� ���콺 ������
    MOUSEDATA stCurrentData;
} MOUSEMANAGER;

////////////////////////////////////////////////////////////////////////////////
//
//  �Լ�
//
////////////////////////////////////////////////////////////////////////////////
BOOL kInitializeMouse( void );
BOOL kAccumulateMouseDataAndPutQueue( BYTE bMouseData );
BOOL kActivateMouse( void );
void kEnableMouseInterrupt( void );
BOOL kIsMouseDataInOutputBuffer( void );
BOOL kGetMouseDataFromMouseQueue( BYTE* pbButtonStatus, int* piRelativeX, 
        int* piRelativeY );

#endif /*__MOUSE_H__*/
