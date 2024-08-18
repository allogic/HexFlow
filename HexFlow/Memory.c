#include <stdlib.h>
#include <string.h>

#include <HexFlow/Memory.h>

long long unsigned g_Allocated = 0;

void* HF_MemoryAlloc(long long unsigned Size, void const *Reference)
{
	long long unsigned NewSize = sizeof(long long unsigned) + Size;

	long long unsigned *NewBlock = (long long unsigned*)calloc(1, NewSize);

	g_Allocated += NewSize;

	*NewBlock = NewSize;
	NewBlock += 1;

	if (Reference)
	{
		memcpy(NewBlock, Reference, Size);
	}

	return NewBlock;
}

void* HF_MemoryRealloc(void *Block, long long unsigned Size)
{
	long long unsigned *PrevBlock = (long long unsigned*)Block;

	PrevBlock -= 1;

	long long unsigned PrevSize = *PrevBlock;
	long long unsigned NewSize = sizeof(long long unsigned) + Size;

	g_Allocated -= PrevSize;

	long long unsigned *NewBlock = (long long unsigned*)realloc(PrevBlock, NewSize);

	if (NewSize > PrevSize)
	{
		memset(((char unsigned*)NewBlock) + PrevSize, 0, NewSize - PrevSize);
	}

	g_Allocated += NewSize;

	*NewBlock = NewSize;
	NewBlock += 1;

	return NewBlock;
}

void HF_MemoryFree(void *Block)
{
	long long unsigned* PrevBlock = (long long unsigned*)Block;

	PrevBlock -= 1;

	long long unsigned PrevSize = *PrevBlock;

	g_Allocated -= PrevSize;

	free(PrevBlock);
}