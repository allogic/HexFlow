#ifndef HF_CONTAINER_VECTOR_H
#define HF_CONTAINER_VECTOR_H

struct HF_Vector* HF_VectorAlloc(long long unsigned ValueSize);
void HF_VectorFree(struct HF_Vector *Vector);

void HF_VectorPush(struct HF_Vector *Vector, void *Item);
void HF_VectorPop(struct HF_Vector *Vector, void *Item);

void HF_VectorResize(struct HF_Vector *Vector, long long unsigned Count);
void HF_VectorClear(struct HF_Vector *Vector);

void* HF_VectorAt(struct HF_Vector *Vector, long long unsigned Index);
void* HF_VectorBuffer(struct HF_Vector *Vector);

char unsigned HF_VectorEmpty(struct HF_Vector *Vector);
long long unsigned HF_VectorCount(struct HF_Vector *Vector);
long long unsigned HF_VectorSize(struct HF_Vector *Vector);

#endif // HF_CONTAINER_VECTOR_H