/**
 *  file    Task.h
 *  date    2009/02/19
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   �½�ũ�� ó���ϴ� �Լ��� ���õ� ����
 */

#pragma once

#include "windef.h"
#include "kList.h"
#include "Synchronization.h"

////////////////////////////////////////////////////////////////////////////////
//
// ��ũ��
//
////////////////////////////////////////////////////////////////////////////////
// SS, RSP, RFLAGS, CS, RIP + ISR���� �����ϴ� 19���� ��������
#define TASK_REGISTERCOUNT     ( 5 + 19 )
#define TASK_REGISTERSIZE       8

// Context �ڷᱸ���� �������� ������
#define TASK_GSOFFSET           0
#define TASK_FSOFFSET           1
#define TASK_ESOFFSET           2
#define TASK_DSOFFSET           3
#define TASK_R15OFFSET          4
#define TASK_R14OFFSET          5
#define TASK_R13OFFSET          6
#define TASK_R12OFFSET          7
#define TASK_R11OFFSET          8
#define TASK_R10OFFSET          9
#define TASK_R9OFFSET           10
#define TASK_R8OFFSET           11
#define TASK_RSIOFFSET          12
#define TASK_RDIOFFSET          13
#define TASK_RDXOFFSET          14
#define TASK_RCXOFFSET          15
#define TASK_RBXOFFSET          16
#define TASK_RAXOFFSET          17
#define TASK_RBPOFFSET          18
#define TASK_RIPOFFSET          19
#define TASK_CSOFFSET           20
#define TASK_RFLAGSOFFSET       21
#define TASK_RSPOFFSET          22
#define TASK_SSOFFSET           23

// �½�ũ Ǯ�� ��巹��
#define TASK_TCBPOOLADDRESS     0x800000
#define TASK_MAXCOUNT           1024

// ���� Ǯ�� ������ ũ��
#define TASK_STACKPOOLADDRESS   ( TASK_TCBPOOLADDRESS + sizeof( TCB ) * TASK_MAXCOUNT )
// ������ �⺻ ũ�⸦ 64Kbyte�� ����
#define TASK_STACKSIZE          ( 64 * 1024 )

// ��ȿ���� ���� �½�ũ ID
#define TASK_INVALIDID          0xFFFFFFFFFFFFFFFF

// �½�ũ�� �ִ�� �� �� �ִ� ���μ��� �ð�(5 ms)
#define TASK_PROCESSORTIME      5

// �غ� ����Ʈ�� ��
#define TASK_MAXREADYLISTCOUNT  5

// �½�ũ�� �켱 ����
#define TASK_FLAGS_HIGHEST            0
#define TASK_FLAGS_HIGH               1
#define TASK_FLAGS_MEDIUM             2
#define TASK_FLAGS_LOW                3
#define TASK_FLAGS_LOWEST             4
#define TASK_FLAGS_WAIT               0xFF          

// �½�ũ�� �÷���
#define TASK_FLAGS_ENDTASK            0x8000000000000000
#define TASK_FLAGS_SYSTEM             0x4000000000000000
#define TASK_FLAGS_PROCESS            0x2000000000000000
#define TASK_FLAGS_THREAD             0x1000000000000000
#define TASK_FLAGS_IDLE               0x0800000000000000
#define TASK_FLAGS_USERLEVEL          0x0400000000000000

// �Լ� ��ũ��
#define GETPRIORITY( x )            ( ( x ) & 0xFF )
#define SETPRIORITY( x, priority )  ( ( x ) = ( ( x ) & 0xFFFFFFFFFFFFFF00 ) | \
        ( priority ) )
#define GETTCBOFFSET( x )           ( ( x ) & 0xFFFFFFFF )

// �ڽ� ������ ��ũ�� ����� stThreadLink �������� �½�ũ �ڷᱸ��(TCB) ��ġ�� 
// ����Ͽ� ��ȯ�ϴ� ��ũ��
#define GETTCBFROMTHREADLINK( x )   ( TCB* ) ( ( DWORD ) ( x ) - offsetof( TCB, \
                                      stThreadLink ) )

// ���μ��� ģȭ�� �ʵ忡 �Ʒ��� ���� �����Ǹ�, �ش� �½�ũ�� Ư���� �䱸������ ���� 
// ������ �Ǵ��ϰ� �½�ũ ���� �л� ����
#define TASK_LOADBALANCINGID    0xFF

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// ���ؽ�Ʈ�� ���õ� �ڷᱸ��
typedef struct kContextStruct
{
    DWORD vqRegister[ TASK_REGISTERCOUNT ];
} CONTEXT;

// �½�ũ(���μ��� �� ������)�� ���¸� �����ϴ� �ڷᱸ��
// FPU ���ؽ�Ʈ�� �߰��Ǿ��� ������ �ڷᱸ���� ũ�Ⱑ 16�� ����� ���ĵǾ�� ��
typedef struct kTaskControlBlockStruct
{
    // ���� �������� ��ġ�� ID
    LISTLINK stLink;
    
    // �÷���
    DWORD dwFlags;
    
    // ���μ��� �޸� ������ ���۰� ũ��
    void* pvMemoryAddress;
    DWORD dwMemorySize;

    //==========================================================================
    // ���� ������ ����
    //==========================================================================
    // �ڽ� �������� ��ġ�� ID
    LISTLINK stThreadLink;
    
    // �θ� ���μ����� ID
    DWORD dwParentProcessID;
    
    // FPU ���ؽ�Ʈ�� 16�� ����� ���ĵǾ�� �ϹǷ�, ������ �߰��� �����ʹ� ���� ����
    // �Ʒ��� �߰��ؾ� ��
    DWORD vdwFPUContext[ 512 / 8 ]; 

    // �ڽ� �������� ����Ʈ
    LIST stChildThreadList;

    // ���ؽ�Ʈ�� ����
    CONTEXT stContext;
    void* pvStackAddress;
    DWORD dwStackSize;
    
    // FPU ��� ����
    BOOL bFPUUsed;
    
    // ���μ��� ģȭ��(Affinity)
    BYTE bAffinity; 
    
    // ���� �½�ũ�� �����ϴ� �ھ��� ���� APIC ID
    BYTE bAPICID;

    // TCB ��ü�� 16����Ʈ ����� ���߱� ���� �е�
    char vcPadding[ 9 ];
} TCB;

// TCB Ǯ�� ���¸� �����ϴ� �ڷᱸ��
typedef struct kTCBPoolManagerStruct
{
    // �ڷᱸ�� ����ȭ�� ���� ���ɶ�
    SPINLOCK stSpinLock;
    
    // �½�ũ Ǯ�� ���� ����
    TCB* pstStartAddress;
    int iMaxCount;
    int iUseCount;
    
    // TCB�� �Ҵ�� Ƚ��
    int iAllocatedCount;
} TCBPOOLMANAGER;

// �����ٷ��� ���¸� �����ϴ� �ڷᱸ��
typedef struct kSchedulerStruct
{
    // �ڷᱸ�� ����ȭ�� ���� ���ɶ�
    SPINLOCK stSpinLock;
    
    // ���� ���� ���� �½�ũ
    TCB* pstRunningTask;
    
    // ���� ���� ���� �½�ũ�� ����� �� �ִ� ���μ��� �ð�
    int iProcessorTime;
    
    // ������ �½�ũ�� �غ����� ����Ʈ, �½�ũ�� �켱 ������ ���� ����
    LIST vstReadyList[ TASK_MAXREADYLISTCOUNT ];

    // ������ �½�ũ�� ������� ����Ʈ
    LIST stWaitList;
    
    // �� �켱 �������� �½�ũ�� ������ Ƚ���� �����ϴ� �ڷᱸ��
    int viExecuteCount[ TASK_MAXREADYLISTCOUNT ];
    
    // ���μ��� ���ϸ� ����ϱ� ���� �ڷᱸ��
    DWORD dwProcessorLoad;
    
    // ���� �½�ũ(Idle Task)���� ����� ���μ��� �ð�
    DWORD dwSpendProcessorTimeInIdleTask;
    
    // ���������� FPU�� ����� �½�ũ�� ID
    DWORD dwLastFPUUsedTaskID;
    
    // ���� �л� ��� ��� ����
    BOOL bUseLoadBalancing;
} SCHEDULER;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
//  �½�ũ Ǯ�� �½�ũ ����
//==============================================================================
static void kInitializeTCBPool( void );
static TCB* kAllocateTCB( void );
static void kFreeTCB(DWORD dwID );
TCB* kCreateTask( DWORD dwFlags, void* pvMemoryAddress, DWORD dwMemorySize, 
                  DWORD dwEntryPointAddress, BYTE bAffinity );
static void kSetUpTask( TCB* pstTCB, DWORD dwFlags, DWORD dwEntryPointAddress,
        void* pvStackAddress, DWORD dwStackSize );

//==============================================================================
//  �����ٷ� ����
//==============================================================================
void kInitializeScheduler( void );
void kSetRunningTask( BYTE bAPICID, TCB* pstTask );
TCB* kGetRunningTask( BYTE bAPICID );
static TCB* kGetNextTaskToRun( BYTE bAPICID );
static BOOL kAddTaskToReadyList( BYTE bAPICID, TCB* pstTask );
BOOL kSchedule( void );
BOOL kScheduleInInterrupt( void );
void kDecreaseProcessorTime( BYTE bAPICID );
BOOL kIsProcessorTimeExpired( BYTE bAPICID );
static TCB* kRemoveTaskFromReadyList( BYTE bAPICID, DWORD dwTaskID );
static BOOL kFindSchedulerOfTaskAndLock( DWORD dwTaskID, BYTE* pbAPICID );
BOOL kChangePriority(DWORD dwID, BYTE bPriority );
BOOL kEndTask(DWORD dwTaskID );
void kExitTask( void );
int kGetReadyTaskCount( BYTE bAPICID );
int kGetTaskCount( BYTE bAPICID );
TCB* kGetTCBInTCBPool( int iOffset );
BOOL kIsTaskExist(DWORD dwID );
DWORD kGetProcessorLoad( BYTE bAPICID );
static TCB* kGetProcessByThread( TCB* pstThread );
void kAddTaskToSchedulerWithLoadBalancing( TCB* pstTask );
static BYTE kFindSchedulerOfMinumumTaskCount( const TCB* pstTask );
BYTE kSetTaskLoadBalancing( BYTE bAPICID, BOOL bUseLoadBalancing );
BOOL kChangeProcessorAffinity(DWORD dwTaskID, BYTE bAffinity );

//==============================================================================
//  ���� �½�ũ ����
//==============================================================================
void kIdleTask( void );
void kHaltProcessorByLoad( BYTE bAPICID );

//==============================================================================
//  FPU ����
//==============================================================================
DWORD kGetLastFPUUsedTaskID( BYTE bAPICID );
void kSetLastFPUUsedTaskID( BYTE bAPICID, DWORD dwTaskID );
