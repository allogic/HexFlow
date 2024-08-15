#include <stdlib.h>

#include <HexFlow/FileSystem.h>
#include <HexFlow/List.h>
#include <HexFlow/Memory.h>

#include <HexFlow/NodeGraph/Graph.h>
#include <HexFlow/NodeGraph/Node.h>
#include <HexFlow/NodeGraph/NodeTypes.h>

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
		HF_Vector2 Size = { 40.0F, 40.0F };

		struct HF_Node *Node = HF_GraphAddNode(Graph, HF_NODE_TYPE_HEX_VIEW, Position, Size, "Memory Dump");

		HF_NodeSetBuffer(Node, Graph->Buffer);
		HF_NodeSetBufferOffset(Node, 0);
		HF_NodeSetBufferSize(Node, 1024);
	}

	{
		HF_Vector3 Position = { 50.0F, 0.0F, 0.0F };
		HF_Vector2 Size = { 60.0F, 30.0F };

		struct HF_Node *Node = HF_GraphAddNode(Graph, HF_NODE_TYPE_TEXT_EDITOR, Position, Size, "Text Editor");

		HF_NodeSetBuffer(Node, 0);
		HF_NodeSetBufferOffset(Node, 0);
		HF_NodeSetBufferSize(Node, 0);
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

struct HF_TextEditorNode* HF_GraphGetTextEditor(struct HF_Graph *Graph)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_NodeType NodeType = HF_NodeGetType(Node);

		if (NodeType == HF_NODE_TYPE_TEXT_EDITOR)
		{
			return (struct HF_TextEditorNode*)Node;
		}
	}

	return 0;
}

void HF_GraphUpdate(struct HF_Graph *Graph)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_NodeUpdate(Node);
	}
}

void HF_GraphDrawFont(struct HF_Graph *Graph, struct HF_Font *Font)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_NodeDrawFont(Node, Font);
	}
}

void HF_GraphDrawLines(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_NodeDrawLines(Node, Gizmos);
	}
}

void HF_GraphDrawQuads(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_NodeDrawQuads(Node, Gizmos);
	}
}

void HF_GraphDrawLineBatch(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_NodeDrawLineBatch(Node, Gizmos);
	}
}

void HF_GraphDrawQuadBatch(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos)
{
	HF_ListEntry *ListEntry = Graph->NodeList.Flink;
	while (ListEntry != &Graph->NodeList)
	{
		struct HF_Node *Node = (struct HF_Node*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_NodeDrawQuadBatch(Node, Gizmos);
	}
}

struct HF_Node* HF_GraphAddNode(struct HF_Graph *Graph, HF_NodeType NodeType, HF_Vector3 Position, HF_Vector2 Size, char const *Title)
{
	struct HF_Node *Node = HF_NodeAlloc(NodeType, Title, Position, Size);

	HF_ListInsertTail(&Graph->NodeList, (HF_ListEntry*)Node);

	return Node;
}