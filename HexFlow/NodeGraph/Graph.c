#include <stdlib.h>

#include <HexFlow/FileSystem.h>
#include <HexFlow/List.h>
#include <HexFlow/Memory.h>

#include <HexFlow/NodeGraph/Graph.h>
#include <HexFlow/NodeGraph/Node.h>

struct HF_Graph
{
	HF_ListEntry ListEntry;
	HF_ListEntry NodeList;
	HF_ListEntry OperationList;

	char unsigned* Buffer;
	long long unsigned BufferSize;
};

struct HF_Graph* HF_GraphAlloc(char const *FileName)
{
	struct HF_Graph *Graph = (struct HF_Graph*)HF_MemoryAlloc(sizeof(struct HF_Graph), 0);

	HF_ListInitHead(&Graph->NodeList);
	HF_ListInitHead(&Graph->OperationList);

	HF_FileSystemReadText(FileName, &Graph->Buffer, &Graph->BufferSize);

	HF_GraphAddNode(Graph, 0, 64);

	return Graph;
}

void HF_GraphFree(struct HF_Graph *Graph)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_NodeFree(Node);
	}

	HF_MemoryFree(Graph->Buffer);
	HF_MemoryFree(Graph);
}

void HF_GraphUpdate(struct HF_Graph *Graph)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		HF_NodeUpdate(Node);

		ListEntry = ListEntry->Flink;
	}
}

void HF_GraphDrawGizmos(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		HF_NodeDrawGizmos(Node, Gizmos);

		ListEntry = ListEntry->Flink;
	}
}

void HF_GraphDrawFont(struct HF_Graph *Graph, struct HF_Font *Font)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		HF_NodeDrawFont(Node, Font);

		ListEntry = ListEntry->Flink;
	}
}

void HF_GraphAddNode(struct HF_Graph *Graph, long long unsigned Offset, long long unsigned Size)
{
	HF_Vector3 position = { 0.0F, 0.0F, 0.0F };
	HF_Vector2 size = { 0.0F, 0.0F };

	struct HF_Node *Node = HF_NodeAlloc(position, size, Graph->Buffer, Offset, Size);

	HF_ListInsertTail(&Graph->NodeList, (HF_ListEntry*)Node);
}