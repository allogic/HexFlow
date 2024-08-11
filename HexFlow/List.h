#ifndef HF_LIST_H
#define HF_LIST_H

typedef struct _HF_ListEntry
{
	struct _HF_ListEntry *Flink;
	struct _HF_ListEntry *Blink;
} HF_ListEntry;

void HF_ListInitHead(HF_ListEntry *List);
void HF_ListInsertTail(HF_ListEntry *List, HF_ListEntry *Entry);
char unsigned HF_ListIsEmpty(HF_ListEntry *List);
HF_ListEntry* HF_ListRemoveHead(HF_ListEntry *List);
long long unsigned HF_ListCount(HF_ListEntry *List);

#endif // HF_LIST_H