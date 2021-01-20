#ifndef _MemoryMgr_hpp_
#define _MemoryMgr_hpp_
#include <stdlib.h>
#include <mutex>
#define Max_MEMORY_SIZE 64
using namespace std;
class MemoryAlloc;
//�ڴ�� ��С��Ԫ
class MemoryBlock
{
public:
	//�ڴ����
	int nID;
	//���ô���
	int nRef;
	//�����ڴ��
	MemoryAlloc* pAlloc;
	//��һ��λ��
	MemoryBlock* pNext;
	//�Ƿ����ڴ����
	bool bPool;
private:
	//Ԥ��
	char c1;
	char c2;
	char c3;
};

//�ڴ��
class MemoryAlloc
{
public:
	MemoryAlloc()
	{
		_pBuf = nullptr;
		_pHeader = nullptr;
		_nSize = 0;
		_nBlockSize = 0;
	}
	~MemoryAlloc()
	{
		if (_pBuf)
		{
			free(_pBuf);
		}
	}
	//�����ڴ�
	void *allocMemory(size_t size)
	{
		if (!_pBuf)
		{
			initMemory();
		}
		MemoryBlock* pReturn = nullptr;
		if (nullptr == _pHeader)
		{
			pReturn = (MemoryBlock*)malloc(size + sizeof(MemoryBlock));
			pReturn->bPool = false;
			pReturn->nID = -1;
			pReturn->nRef = 1;
			pReturn->pAlloc = nullptr;
			pReturn->pNext = nullptr;
		}
		else
		{
			pReturn = _pHeader;
			_pHeader = _pHeader->pNext;
			pReturn->nRef = 1;
		}
		return ((char*)pReturn + sizeof(MemoryBlock));
	}
	//�ͷ��ڴ�
	void freeMemory(void*  pMem)
	{
	
		MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
		if (--pBlock->nRef !=0)
		{
			return;
		}
		if (pBlock->bPool)
		{
			pBlock->pNext = _pHeader;
			_pHeader = pBlock;
		}
		else
		{
			free(pBlock);
		}
	}
	//��ʼ���ڴ��
	void initMemory()
	{
		if (_pBuf)
		{
			return;
		}
		//�����ڴ�صĴ�С
		size_t bufSize = (_nSize + sizeof(MemoryBlock))* _nBlockSize;
		//��ϵͳ������ڴ�
		_pBuf = (char*)malloc(bufSize);
		//��ʼ���ڴ��
		_pHeader = (MemoryBlock*)_pBuf;
		_pHeader->bPool = true;
		_pHeader->nID = 0;
		_pHeader->nRef = 0;
		_pHeader->pAlloc = this;
		_pHeader->pNext = nullptr;
		//�����ڴ���ʼ����Ϣ
		MemoryBlock* pTem1 = _pHeader;
		for (size_t i = 1; i < _nBlockSize; i++)
		{
			MemoryBlock* pTem2 = (MemoryBlock*)(_pBuf+(i * _nSize));
			pTem2->bPool = true;
			pTem2->nID = i;
			pTem2->nRef = 0;
			pTem2->pAlloc = this;
			pTem2->pNext = nullptr;
			pTem1->pNext = pTem2;
			pTem1 = pTem2;
		}
	}
protected:
	//�ڴ�ص�ַ
	char* _pBuf;
	//ͷ���ڴ浥Ԫ
	MemoryBlock* _pHeader;
	//�ڴ浥Ԫ��С
	size_t _nSize;
	//�ڴ浥Ԫ����
	size_t _nBlockSize;
};
template<size_t nSize,size_t nBlockSize>
class MemoryAlloctor : public MemoryAlloc
{
public:
	MemoryAlloctor()
	{
		const size_t n = sizeof(void*);
		_nSize = (nSize/n) * n + (nSize % n ? n : 0) ;
		_nBlockSize = nBlockSize;
	}
};
//�ڴ�ع�����
class MemoryMgr
{
private:
	MemoryMgr()
	{
		Init(0,64,&_Memory64);
	}
	~MemoryMgr()
	{

	}

public:
	
	static MemoryMgr &Instance()
	{
		static MemoryMgr mgr;
		return mgr;
	}
	//�����ڴ�
	void *allocMem(size_t size)
	{
		if (size <=Max_MEMORY_SIZE)
		{
			return _szAlloc[size]->allocMemory(size);
		}
		else
		{
			MemoryBlock* pReturn = (MemoryBlock*)malloc(size + sizeof(MemoryBlock));
			pReturn->bPool = false;
			pReturn->nID = -1;
			pReturn->nRef = 1;
			pReturn->pAlloc = nullptr;
			pReturn->pNext = nullptr;
			return ((char*)pReturn + sizeof(MemoryBlock));

		}
	}
	//�ͷ��ڴ�
	void freeMem(void*  pMem)
	{
		MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
		if (pBlock->bPool)
		{
			pBlock->pAlloc->freeMemory(pMem);
		}
		else
		{
			if (--pBlock->nRef ==0)
			{
				free(pBlock);
			}
		}
	}
	//���������ڴ�����
	void addRef(void* pMem)
	{
		MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
		++pBlock->nRef;
	}
private:
	//��ʼ���ڴ��ӳ������
	void Init(int nBegin,int nEnd,MemoryAlloc* pMem)
	{
		for (int i = nBegin; i < nEnd; i++)
		{
			_szAlloc[i] = pMem;
		}
	}
private:
	MemoryAlloctor<64, 10>_Memory64;
	MemoryAlloc* _szAlloc[Max_MEMORY_SIZE + 1];
};


#endif // !_MemoryMgr_hpp_
