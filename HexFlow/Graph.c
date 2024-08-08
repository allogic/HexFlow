#include <stdlib.h>

#include <HexFlow/FileSystem.h>
#include <HexFlow/Font.h>
#include <HexFlow/Graph.h>

struct HF_Graph
{
	HF_ListEntry ListEntry;

	char unsigned *Buffer;
	long long unsigned Size;

	HF_ListEntry OperationList;
};

struct HF_Graph* HF_GraphAlloc(char const *FileName)
{
	struct HF_Graph *Graph = (struct HF_Graph*)calloc(1, sizeof(struct HF_Graph));

	HF_FileSystemReadFile("r", FileName, &Graph->Buffer, &Graph->Size);

	return Graph;
}

void HF_GraphFree(struct HF_Graph *Graph)
{
	free(Graph->Buffer);
	free(Graph);
}

HF_ListEntry* HF_GraphListEntry(struct HF_Graph *Graph)
{
	return &Graph->ListEntry;
}

void HF_GraphDraw(struct HF_Graph *Graph, struct HF_Font *Font)
{
	HF_FontDraw(Font, 0.0F, 0.0F, Graph->Buffer);
}