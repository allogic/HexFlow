#include <string.h>

#include <HexFlow/Font.h>
#include <HexFlow/Gizmos.h>
#include <HexFlow/List.h>
#include <HexFlow/Memory.h>

#include <HexFlow/Math/Vector.h>

#include <HexFlow/NodeGraph/Node.h>

#define HF_NODE_TITLE_SIZE 64

struct HF_Node
{
	HF_ListEntry ListEntry;

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

	char unsigned *Buffer;

	long long unsigned BufferOffset;
	long long unsigned BufferSize;
};

struct HF_Node* HF_NodeAlloc(char const *Title, HF_Vector3 Position, HF_Vector2 Size, char unsigned *Buffer, long long unsigned BufferOffset, long long unsigned BufferSize)
{
	struct HF_Node *Node = (struct HF_Node*)HF_MemoryAlloc(sizeof(struct HF_Node), 0);

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
	Node->WindowBorderColor = 0x555555FF;
	Node->ContentBorderColor = 0x555555FF;

	Node->Buffer = Buffer;

	Node->BufferOffset = BufferOffset;
	Node->BufferSize = BufferSize;

	return Node;
}

void HF_NodeFree(struct HF_Node *Node)
{
	HF_MemoryFree(Node);
}

void HF_NodeUpdate(struct HF_Node *Node)
{

}

void HF_NodeDrawLines(struct HF_Node *Node, struct HF_Gizmos *Gizmos)
{

}

void HF_NodeDrawQuads(struct HF_Node *Node, struct HF_Gizmos *Gizmos)
{

}

void HF_NodeDrawLineBatch(struct HF_Node *Node, struct HF_Gizmos *Gizmos)
{
	HF_Vector3 WindowBorderPosition =
	{
		Node->Position[0],
		Node->Position[1] - Node->Size[1] + (Node->MarginTop + Node->PaddingTop) - (Node->MarginBottom + Node->PaddingBottom),
		Node->Position[2] + 0.05F,
	};

	HF_Vector2 WindowBorderSize =
	{
		Node->Size[0] + (Node->MarginLeft + Node->PaddingLeft) + (Node->MarginRight + Node->PaddingRight),
		Node->Size[1] - (Node->MarginTop + Node->PaddingTop) + (Node->MarginBottom + Node->PaddingBottom),
	};

	HF_GizmosDrawBatchLineRect(Gizmos, WindowBorderPosition, WindowBorderSize, Node->WindowBorderColor);

	HF_Vector3 ContentBorderPosition =
	{
		Node->Position[0] + Node->MarginLeft,
		Node->Position[1] - Node->Size[1] + Node->MarginTop + Node->PaddingTop - Node->PaddingBottom,
		Node->Position[2] + 0.05F,
	};

	HF_Vector2 ContentBorderSize =
	{
		Node->Size[0] + Node->PaddingLeft + Node->PaddingRight,
		Node->Size[1] - (Node->MarginTop * 2.0F) - Node->PaddingTop + Node->PaddingBottom,
	};

	HF_GizmosDrawBatchLineRect(Gizmos, ContentBorderPosition, ContentBorderSize, Node->ContentBorderColor);
}

void HF_NodeDrawQuadBatch(struct HF_Node *Node, struct HF_Gizmos *Gizmos)
{
	HF_Vector3 BackgroundPosition =
	{
		Node->Position[0],
		Node->Position[1] - Node->Size[1] + (Node->MarginTop + Node->PaddingTop) - (Node->MarginBottom + Node->PaddingBottom),
		Node->Position[2] + 0.1F,
	};

	HF_Vector2 BackgroundSize =
	{
		Node->Size[0] + (Node->MarginLeft + Node->PaddingLeft) + (Node->MarginRight + Node->PaddingRight),
		Node->Size[1] - (Node->MarginTop + Node->PaddingTop) + (Node->MarginBottom + Node->PaddingBottom),
	};

	HF_GizmosDrawBatchQuad(Gizmos, BackgroundPosition, BackgroundSize, Node->WindowBackgroundColor);
}

void HF_NodeDrawFont(struct HF_Node *Node, struct HF_Font *Font)
{
	HF_Vector3 ContentPosition =
	{
		Node->Position[0] + (Node->MarginLeft + Node->PaddingLeft),
		Node->Position[1] - (Node->MarginTop + Node->PaddingTop),
		Node->Position[2],
	};

	HF_Vector2 ContentSize =
	{
		0.0F,
		0.0F,
	};

	HF_FontDrawHex(Font, ContentPosition, ContentSize, Node->Buffer + Node->BufferOffset, Node->BufferSize, 16, 0.4F);

	HF_Vector2Set(ContentSize, Node->Size);

	HF_Vector3 TitlePosition =
	{
		Node->Position[0] + 1.0F,
		Node->Position[1] - 0.75F,
		Node->Position[2],
	};

	HF_Vector2 TitleSize =
	{
		0.0F,
		0.0F,
	};

	HF_FontDrawText(Font, TitlePosition, TitleSize, Node->Title);
}