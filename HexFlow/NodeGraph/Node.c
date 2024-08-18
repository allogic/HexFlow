#include <string.h>

#include <HexFlow/Font.h>
#include <HexFlow/Gizmos.h>
#include <HexFlow/List.h>
#include <HexFlow/Memory.h>

#include <HexFlow/Math/Vector.h>

#include <HexFlow/NodeGraph/Node.h>

#include <HexFlow/NodeGraph/Nodes/RootNode.h>
#include <HexFlow/NodeGraph/Nodes/TextEditorNode.h>

#define HF_NODE_TITLE_SIZE 64

struct HF_Node
{
	HF_ListEntry ListEntry;

	HF_NodeType NodeType;

	char Title[HF_NODE_TITLE_SIZE];

	HF_Vector3 Position;
	HF_Vector2 Size;
	
	float MarginLeft;
	float MarginRight;
	float MarginTop;
	float MarginBottom;

	float PaddingLeft;
	float PaddingRight;
	float PaddingTop;
	float PaddingBottom;

	int unsigned WindowBackgroundColor;
	int unsigned WindowBorderColor;
	int unsigned ContentBorderColor;

	void* VirtualNode;
};

struct HF_Node* HF_NodeAlloc(HF_NodeType NodeType, char const *Title, HF_Vector3 Position, HF_Vector2 Size)
{
	struct HF_Node *Node = (struct HF_Node*)HF_MemoryAlloc(sizeof(struct HF_Node), 0);

	Node->NodeType = NodeType;

	strcpy(Node->Title, Title);

	HF_Vector3Set(Position, Node->Position);
	HF_Vector2Set(Size, Node->Size);

	Node->MarginLeft = 1.0F;
	Node->MarginRight = 1.0F;
	Node->MarginTop = 3.0F;
	Node->MarginBottom = 1.0F;

	Node->PaddingLeft = 0.5F;
	Node->PaddingRight = 0.5F;
	Node->PaddingTop = 0.3F;
	Node->PaddingBottom = 0.5F;

	Node->WindowBackgroundColor = 0x222222FF;
	Node->WindowBorderColor = 0x383838FF;
	Node->ContentBorderColor = 0x555555FF;

	switch (Node->NodeType)
	{
		case HF_NODE_TYPE_ROOT: Node->VirtualNode = HF_RootNodeAlloc(Node); break;
		case HF_NODE_TYPE_TEXT_EDITOR: Node->VirtualNode = HF_TextEditorNodeAlloc(Node); break;
	}

	return Node;
}

void HF_NodeFree(struct HF_Node *Node)
{
	switch (Node->NodeType)
	{
		case HF_NODE_TYPE_ROOT: HF_RootNodeFree(Node->VirtualNode); break;
		case HF_NODE_TYPE_TEXT_EDITOR: HF_TextEditorNodeFree(Node->VirtualNode); break;
	}

	HF_MemoryFree(Node);
}

HF_NodeType HF_NodeGetType(struct HF_Node *Node)
{
	return Node->NodeType;
}

void HF_NodeGetPosition(struct HF_Node *Node, HF_Vector3 Position)
{
	Position[0] = Node->Position[0];
	Position[1] = Node->Position[1];
	Position[2] = Node->Position[2];
}

void HF_NodeGetSize(struct HF_Node *Node, HF_Vector2 Size)
{
	Size[0] = Node->Size[0];
	Size[1] = Node->Size[1];
}

void HF_NodeGetContentPosition(struct HF_Node *Node, HF_Vector3 Position)
{
	Position[0] = Node->Position[0] + Node->MarginLeft + Node->PaddingLeft;
	Position[1] = Node->Position[1] - Node->MarginTop - Node->PaddingTop;
	Position[2] = Node->Position[2];
}

void HF_NodeGetContentSize(struct HF_Node *Node, HF_Vector2 Size)
{
	Size[0] = Node->Size[0] - Node->MarginLeft - Node->PaddingLeft - Node->MarginRight - Node->PaddingRight;
	Size[1] = Node->Size[1] - Node->MarginTop - Node->PaddingTop - Node->MarginBottom - Node->PaddingBottom;
}

void* HF_NodeGetVirtualNode(struct HF_Node *Node)
{
	return Node->VirtualNode;
}

void HF_NodeUpdate(struct HF_Node *Node)
{
	switch (Node->NodeType)
	{
		case HF_NODE_TYPE_ROOT: HF_RootNodeUpdate(Node->VirtualNode); break;
		case HF_NODE_TYPE_TEXT_EDITOR: HF_TextEditorNodeUpdate(Node->VirtualNode); break;
	}
}

void HF_NodeDrawFont(struct HF_Node *Node, struct HF_Font *Font)
{
	HF_Vector2 TitleSize =
	{
		Node->Size[0],
		Node->Size[1],
	};

	HF_Vector3 TitlePosition =
	{
		Node->Position[0] + 1.0F,
		Node->Position[1] - 0.75F,
		Node->Position[2],
	};

	HF_FontDrawTextClipped(Font, TitlePosition, TitleSize, Node->Title);

	switch (Node->NodeType)
	{
		case HF_NODE_TYPE_ROOT: HF_RootNodeDrawFont(Node->VirtualNode, Font); break;
		case HF_NODE_TYPE_TEXT_EDITOR: HF_TextEditorNodeDrawFont(Node->VirtualNode, Font); break;
	}
}

void HF_NodeDrawLines(struct HF_Node *Node, struct HF_Gizmos *Gizmos)
{
	switch (Node->NodeType)
	{
		case HF_NODE_TYPE_ROOT: HF_RootNodeDrawLines(Node->VirtualNode, Gizmos); break;
		case HF_NODE_TYPE_TEXT_EDITOR: HF_TextEditorNodeDrawLines(Node->VirtualNode, Gizmos); break;
	}
}

void HF_NodeDrawQuads(struct HF_Node *Node, struct HF_Gizmos *Gizmos)
{
	switch (Node->NodeType)
	{
		case HF_NODE_TYPE_ROOT: HF_RootNodeDrawQuads(Node->VirtualNode, Gizmos); break;
		case HF_NODE_TYPE_TEXT_EDITOR: HF_TextEditorNodeDrawQuads(Node->VirtualNode, Gizmos); break;
	}
}

void HF_NodeDrawLineBatch(struct HF_Node *Node, struct HF_Gizmos *Gizmos)
{
	HF_Vector2 WindowBorderSize =
	{
		Node->Size[0],
		Node->Size[1],
	};

	HF_Vector3 WindowBorderPosition =
	{
		Node->Position[0],
		Node->Position[1] - WindowBorderSize[1],
		Node->Position[2] + 0.1F,
	};

	HF_GizmosDrawBatchLineRect(Gizmos, WindowBorderPosition, WindowBorderSize, Node->WindowBorderColor);

	HF_Vector2 ContentBorderSize =
	{
		Node->Size[0] - Node->MarginLeft - Node->MarginRight,
		Node->Size[1] - Node->MarginTop - Node->MarginBottom,
	};

	HF_Vector3 ContentBorderPosition =
	{
		Node->Position[0] + Node->MarginLeft,
		Node->Position[1] - ContentBorderSize[1] - Node->MarginTop,
		Node->Position[2] + 0.1F,
	};

	HF_GizmosDrawBatchLineRect(Gizmos, ContentBorderPosition, ContentBorderSize, Node->ContentBorderColor);

	switch (Node->NodeType)
	{
		case HF_NODE_TYPE_ROOT: HF_RootNodeDrawLineBatch(Node->VirtualNode, Gizmos); break;
		case HF_NODE_TYPE_TEXT_EDITOR: HF_TextEditorNodeDrawLineBatch(Node->VirtualNode, Gizmos); break;
	}
}

void HF_NodeDrawQuadBatch(struct HF_Node *Node, struct HF_Gizmos *Gizmos)
{
	HF_Vector2 BackgroundSize =
	{
		Node->Size[0],
		Node->Size[1],
	};

	HF_Vector3 BackgroundPosition =
	{
		Node->Position[0],
		Node->Position[1] - BackgroundSize[1],
		Node->Position[2] + 0.2F,
	};

	HF_GizmosDrawBatchQuad(Gizmos, BackgroundPosition, BackgroundSize, Node->WindowBackgroundColor);

	switch (Node->NodeType)
	{
		case HF_NODE_TYPE_ROOT: HF_RootNodeDrawQuadBatch(Node->VirtualNode, Gizmos); break;
		case HF_NODE_TYPE_TEXT_EDITOR: HF_TextEditorNodeDrawQuadBatch(Node->VirtualNode, Gizmos); break;
	}
}