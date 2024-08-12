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

	{
		HF_Vector3 Position = { 0.0F, 0.0F, 0.0F };
		HF_Vector2 Size = { 0.0F, 0.0F };

		HF_GraphAddNode(Graph, Position, Size, "Memory Dump", 0, 1024);
	}

	{
		HF_Vector3 Position = { 30.0F, 0.0F, 0.0F };
		HF_Vector2 Size = { 0.0F, 0.0F };

		HF_GraphAddNode(Graph, Position, Size, "Text Editor", 1024, 512);
	}

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

void HF_GraphDrawLines(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		HF_NodeDrawLines(Node, Gizmos);

		ListEntry = ListEntry->Flink;
	}
}

void HF_GraphDrawQuads(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		HF_NodeDrawQuads(Node, Gizmos);

		ListEntry = ListEntry->Flink;
	}
}

void HF_GraphDrawLineBatch(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		HF_NodeDrawLineBatch(Node, Gizmos);

		ListEntry = ListEntry->Flink;
	}
}

void HF_GraphDrawQuadBatch(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		HF_NodeDrawQuadBatch(Node, Gizmos);

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

void HF_GraphAddNode(struct HF_Graph *Graph, HF_Vector3 Position, HF_Vector2 Size, char const *Title, long long unsigned BufferOffset, long long unsigned BufferSize)
{
	struct HF_Node *Node = HF_NodeAlloc(Title, Position, Size, Graph->Buffer, BufferOffset, BufferSize);

	HF_ListInsertTail(&Graph->NodeList, (HF_ListEntry*)Node);
}