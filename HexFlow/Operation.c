#include <stdlib.h>

#include <HexFlow/List.h>
#include <HexFlow/Memory.h>
#include <HexFlow/Operation.h>

struct HF_Operation
{
	HF_ListEntry ListEntry;

	char CodeBuffer[0x1000];
};

struct HF_Operation* HF_OperationAlloc(void)
{
	struct HF_Operation *Operation = (struct HF_Operation*)HF_MemoryAlloc(sizeof(struct HF_Operation), 0);



	return Operation;
}

void HF_OperationFree(struct HF_Operation* Operation)
{
	HF_MemoryFree(Operation);
}