#pragma once
#include "windef.h"
#include "VirtualMemoryManager.h"
#include "kheap.h"
#include "DoubleLinkedList.h"

class Thread;

class Process
{
public:
	Process();
	virtual ~Process();

	BOOL AddThread(Thread* pThread); //�����带 �߰��Ѵ�.
	Thread* GetThread(int index); //�����带 �����Ѵ�.

	void SetPDBR();

	UINT32		m_processId; //���μ��� ���̵�
	char		m_processName[256]; //���μ��� �̸�
	UINT32		m_dwRunState; //���μ��� ����
	UINT32		m_dwPriority; //���μ��� �켱����
	int			m_dwRunningTime; // ���μ��� CPU ���� �ð�
	UINT32		m_dwPageCount; //���μ����� �����ϴ� ������ ��
	UINT32		m_dwProcessType; //���μ��� Ÿ��	

	PageDirectory* m_pPageDirectory; //�� ���μ����� ����ϴ� ������ ���丮	
	void*		m_lpHeap; //���μ����� ����ϴ� ��
		
	int			m_kernelStackIndex; //�����尡 �����ɶ� ���� �� ������ �Ҵ��� �־�� �ϴµ� �׶� ���Ǵ� �ε���
	DoubleLinkedList m_threadList; //���μ������� ���� ������ ����Ʈ

	uint32_t	m_imageBase; //���Ϸ� ���� �ڵ带 �ε��� ��� ����. ���Ͽ� �޸𸮿� �ε�� �ּ�
	uint32_t	m_imageSize; //������ ũ��

private:
	
};

