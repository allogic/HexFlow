#ifndef HF_OPERATION_H
#define HF_OPERATION_H

struct HF_Operation* HF_OperationAlloc(void);
void HF_OperationFree(struct HF_Operation *Operation);

#endif // HF_OPERATION_H