#include <stdlib.h>
#include <string.h>

#include <HexFlow/Memory.h>

long long unsigned g_Allocated = 0;

void* HF_MemoryAlloc(long long unsigned Size, void const *Reference)
{

#ifdef HF_DEBUG

	long long unsigned RealSize = sizeof(long long unsigned) + Size;
	long long unsigned *Block = (long long unsigned*)calloc(1, RealSize);

	g_Allocated += RealSize;

	*Block = RealSize;
	Block += 1;

	if (Reference)
	{
		memcpy(Block, Reference, Size);
	}

	return Block;

#else

	return calloc(1, Size);

#endif // HF_DEBUG

}

void HF_MemoryFree(void *Block)
{

#ifdef HF_DEBUG

	long long unsigned* RealBlock = ((long long unsigned*)Block) - 1;

	g_Allocated -= *RealBlock;

	free(RealBlock);

#else

	free(RealBlock);

#endif // HF_DEBUG

}