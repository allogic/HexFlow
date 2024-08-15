#ifndef HF_MEMORY_H
#define HF_MEMORY_H

extern long long unsigned g_Allocated;

void* HF_MemoryAlloc(long long unsigned Size, void const *Reference);
void* HF_MemoryRealloc(void *Block, long long unsigned Size);

void HF_MemoryFree(void *Block);

#endif // HF_MEMORY_H