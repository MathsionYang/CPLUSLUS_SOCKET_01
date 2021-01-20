#ifndef _ALLOCTOR_H
#define _ALLOCTOR_H

void *operator new(size_t size);
void operator delete(void* p);
void *operator new[](size_t size);
void operator delete[](void* p);

void *mem_malloc(size_t size);
void mem_free(void* p);
#endif // !_ALLOCTOR_H

