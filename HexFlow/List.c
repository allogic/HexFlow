#include <HexFlow/List.h>

void HF_ListInitHead(HF_ListEntry *List)
{
	List->Flink = List;
	List->Blink = List;
}

void HF_ListInsertTail(HF_ListEntry *List, HF_ListEntry *Entry)
{
	Entry->Flink = List;
	Entry->Blink = List->Blink;

	List->Blink->Flink = Entry;
	List->Blink = Entry;
}

char unsigned HF_ListIsEmpty(HF_ListEntry *List)
{
	return (List->Flink == List) && (List->Blink == List);
}

HF_ListEntry* HF_ListRemoveHead(HF_ListEntry *List)
{
	HF_ListEntry *Entry = 0;

	if (HF_ListIsEmpty(List))
	{
		return 0;
	}

	Entry = List->Flink;

	List->Flink = Entry->Flink;
	Entry->Flink->Blink = List;

	Entry->Flink = Entry->Blink = 0;

	return Entry;
}

long long unsigned HF_ListCount(HF_ListEntry *List)
{
	long long unsigned Count = 0;

	HF_ListEntry *Entry = List->Flink;
	while (Entry != List)
	{
		Count++;

		Entry = Entry->Flink;
	}

	return Count;
}