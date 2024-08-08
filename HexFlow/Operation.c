#include <stdlib.h>

#include <HexFlow/Operation.h>

struct HF_Operation
{
	HF_ListEntry ListEntry;

	char CodeBuffer[0x1000];
};

struct HF_Operation* HF_OperationAlloc(void)
{
	struct HF_Operation *Operation = (struct HF_Operation*)calloc(1, sizeof(struct HF_Operation));



	return Operation;
}

void HF_OperationFree(struct HF_Operation* Operation)
{
	free(Operation);
}

HF_ListEntry* HF_OperationListEntry(struct HF_Operation *Operation)
{
	return &Operation->ListEntry;
}