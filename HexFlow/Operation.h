#ifndef HF_OPERATION_H
#define HF_OPERATION_H

#include <HexFlow/List.h>

struct HF_Operation* HF_OperationAlloc(void);
void HF_OperationFree(struct HF_Operation *Operation);

HF_ListEntry* HF_OperationListEntry(struct HF_Operation *Operation)

#endif