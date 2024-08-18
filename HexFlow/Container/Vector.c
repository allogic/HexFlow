#include <string.h>

#include <HexFlow/Macros.h>
#include <HexFlow/Memory.h>

#include <HexFlow/Container/Vector.h>

#define HF_VECTOR_INITIAL_CAPACITY 16

struct HF_Vector
{
	char unsigned* Buffer;

	long long unsigned ValueSize;
	long long unsigned BufferSize;
	long long unsigned BufferCount;
	long long unsigned BufferIndex;
	long long unsigned BufferOffset;
};

static void HF_VectorExpand(struct HF_Vector *Vector);

struct HF_Vector* HF_VectorAlloc(long long unsigned ValueSize)
{
	struct HF_Vector *Vector = (struct HF_Vector*)HF_MemoryAlloc(sizeof(struct HF_Vector), 0);

	Vector->Buffer = (char unsigned*)HF_MemoryAlloc(ValueSize * HF_VECTOR_INITIAL_CAPACITY, 0);

	Vector->ValueSize = ValueSize;
	Vector->BufferSize = ValueSize * HF_VECTOR_INITIAL_CAPACITY;
	Vector->BufferCount = HF_VECTOR_INITIAL_CAPACITY;
	Vector->BufferIndex = 0;
	Vector->BufferOffset = 0;

	return Vector;
}

void HF_VectorFree(struct HF_Vector *Vector)
{
	HF_MemoryFree(Vector->Buffer);
	HF_MemoryFree(Vector);
}

void HF_VectorPush(struct HF_Vector *Vector, void *Item)
{
	memcpy(Vector->Buffer + Vector->BufferOffset, Item, Vector->ValueSize);

	Vector->BufferIndex += 1;
	Vector->BufferOffset += Vector->ValueSize;

	if (Vector->BufferIndex >= Vector->BufferCount)
	{
		HF_VectorExpand(Vector);
	}
}

void HF_VectorPop(struct HF_Vector *Vector, void *Item)
{
	Vector->BufferIndex -= 1;
	Vector->BufferOffset -= Vector->ValueSize;

	if (Item)
	{
		memcpy(Item, Vector->Buffer + Vector->BufferOffset, Vector->ValueSize);
	}

	memset(Vector->Buffer + Vector->BufferOffset, 0, Vector->ValueSize);
}

void HF_VectorResize(struct HF_Vector *Vector, long long unsigned Count)
{
	if (Count > Vector->BufferCount)
	{
		Vector->Buffer = (char unsigned*)HF_MemoryRealloc(Vector->Buffer, Count * Vector->ValueSize);
		Vector->BufferCount = Count;
		Vector->BufferSize = Count * Vector->ValueSize;
	}
	else if (Count < Vector->BufferCount)
	{
		Vector->Buffer = (char unsigned*)HF_MemoryRealloc(Vector->Buffer, Count * Vector->ValueSize);
		Vector->BufferCount = Count;
		Vector->BufferSize = Count * Vector->ValueSize;
		Vector->BufferIndex = HF_MIN(Vector->BufferIndex, Count);
		Vector->BufferOffset = HF_MIN(Vector->BufferIndex, Count) * Vector->ValueSize;
	}
}

void HF_VectorClear(struct HF_Vector *Vector)
{
	Vector->BufferIndex = 0;
	Vector->BufferOffset = 0;
}

void* HF_VectorAt(struct HF_Vector *Vector, long long unsigned Index)
{
	return Vector->Buffer + (Index * Vector->ValueSize);
}

void* HF_VectorBuffer(struct HF_Vector *Vector)
{
	return (void*)Vector->Buffer;
}

char unsigned HF_VectorEmpty(struct HF_Vector *Vector)
{
	return Vector->BufferIndex == 0;
}

long long unsigned HF_VectorCount(struct HF_Vector *Vector)
{
	return Vector->BufferIndex;
}

long long unsigned HF_VectorSize(struct HF_Vector *Vector)
{
	return Vector->BufferIndex * Vector->ValueSize;
}

static void HF_VectorExpand(struct HF_Vector *Vector)
{
	long long unsigned NextBufferCount = Vector->BufferCount * 2;
	long long unsigned NextBufferSize = Vector->BufferSize * 2;

	Vector->Buffer = (char unsigned*)HF_MemoryRealloc(Vector->Buffer, NextBufferSize);
	Vector->BufferCount = NextBufferCount;
	Vector->BufferSize = NextBufferSize;
}