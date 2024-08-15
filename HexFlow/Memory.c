#include <stdlib.h>
#include <string.h>

#include <HexFlow/Memory.h>

long long unsigned g_Allocated = 0;

void* HF_MemoryAlloc(long long unsigned Size, void const *Reference)
{

#ifdef HF_DEBUG

	long long unsigned *NewBlock = (long long unsigned*)calloc(1, sizeof(long long unsigned) + Size);

	g_Allocated += sizeof(long long unsigned) + Size;

	*NewBlock = sizeof(long long unsigned) + Size;
	NewBlock += 1;

	if (Reference)
	{
		memcpy(NewBlock, Reference, Size);
	}

	return NewBlock;

#else

	void* Block = calloc(1, Size);

	if (Reference)
	{
		memcpy(Block, Reference, Size);
	}

	return Block;

#endif // HF_DEBUG

}

void* HF_MemoryRealloc(void *Block, long long unsigned Size)
{

#ifdef HF_DEBUG

	long long unsigned *PrevBlock = (long long unsigned*)Block;

	PrevBlock -= 1;

	g_Allocated -= *PrevBlock;

	long long unsigned *NewBlock = (long long unsigned*)realloc(PrevBlock, sizeof(long long unsigned) + Size);

	g_Allocated += sizeof(long long unsigned) + Size;

	*NewBlock = sizeof(long long unsigned) + Size;
	NewBlock += 1;

	return NewBlock;

#else

	return realloc(Block, Size);

#endif // HF_DEBUG

}

void HF_MemoryFree(void *Block)
{

#ifdef HF_DEBUG

	long long unsigned* PrevBlock = (long long unsigned*)Block;

	PrevBlock -= 1;

	g_Allocated -= *PrevBlock;

	free(PrevBlock);

#else

	free(Block);

#endif // HF_DEBUG

}