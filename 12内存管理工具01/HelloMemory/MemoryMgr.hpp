#ifndef _MemoryMgr_hpp_
#define _MemoryMgr_hpp_
#include <stdlib.h>
#include <mutex>
#define Max_MEMORY_SIZE 64
using namespace std;
class MemoryAlloc;
//内存块 最小单元
class MemoryBlock
{
public:
	//内存块编号
	int nID;
	//引用次数
	int nRef;
	//所属内存池
	MemoryAlloc* pAlloc;
	//下一块位置
	MemoryBlock* pNext;
	//是否在内存池中
	bool bPool;
private:
	//预留
	char c1;
	char c2;
	char c3;
};

//内存池
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
	//申请内存
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
	//释放内存
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
	//初始化内存池
	void initMemory()
	{
		if (_pBuf)
		{
			return;
		}
		//计算内存池的大小
		size_t bufSize = (_nSize + sizeof(MemoryBlock))* _nBlockSize;
		//向系统申请池内存
		_pBuf = (char*)malloc(bufSize);
		//初始化内存池
		_pHeader = (MemoryBlock*)_pBuf;
		_pHeader->bPool = true;
		_pHeader->nID = 0;
		_pHeader->nRef = 0;
		_pHeader->pAlloc = this;
		_pHeader->pNext = nullptr;
		//遍历内存块初始化信息
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
	//内存池地址
	char* _pBuf;
	//头部内存单元
	MemoryBlock* _pHeader;
	//内存单元大小
	size_t _nSize;
	//内存单元数量
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
//内存池管理工具
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
	//申请内存
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
	//释放内存
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
	//增加引用内存块计数
	void addRef(void* pMem)
	{
		MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
		++pBlock->nRef;
	}
private:
	//初始化内存池映射数组
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
