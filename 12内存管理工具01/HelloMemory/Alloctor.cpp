#include "Alloctor.h"
#include "MemoryMgr.hpp"

void *operator new(size_t size)
{
	return MemoryMgr::Instance().allocMem(size);
		
}
void operator delete(void* p)
{
	return MemoryMgr::Instance().freeMem(p);
}


void *operator new[](size_t size)
{
	return MemoryMgr::Instance().allocMem(size);
}
void operator delete[](void* p)
{
	 MemoryMgr::Instance().freeMem(p);
}


void *mem_malloc(size_t size)
{
	return MemoryMgr::Instance().allocMem(size);
}
void mem_free(void* p)
{
	 MemoryMgr::Instance().freeMem(p);
}
