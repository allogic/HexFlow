#include <HexFlow/Font.h>
#include <HexFlow/Gizmos.h>
#include <HexFlow/List.h>
#include <HexFlow/Memory.h>

#include <HexFlow/Math/Vector.h>

#include <HexFlow/NodeGraph/Node.h>

struct HF_Node
{
	HF_ListEntry ListEntry;

	HF_Vector3 Position;
	HF_Vector2 Size;

	int unsigned Color;

	char unsigned *Buffer;

	long long unsigned BufferOffset;
	long long unsigned BufferSize;
};

struct HF_Node* HF_NodeAlloc(HF_Vector3 Position, HF_Vector2 Size, char unsigned *Buffer, long long unsigned BufferOffset, long long unsigned BufferSize)
{
	struct HF_Node *Node = (struct HF_Node*)HF_MemoryAlloc(sizeof(struct HF_Node), 0);

	HF_Vector3Set(Position, Node->Position);
	HF_Vector2Set(Size, Node->Size);

	Node->Color = 0x222222FF;
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

void HF_NodeDrawGizmos(struct HF_Node *Node, struct HF_Gizmos *Gizmos)
{
	HF_Vector3 Position = { Node->Position[0], Node->Position[1], Node->Position[2] + 0.1F };

	HF_GizmosDrawQuad(Gizmos, Position, Node->Size, Node->Color);
}

void HF_NodeDrawFont(struct HF_Node *Node, struct HF_Font *Font)
{
	HF_Vector3 Position = { Node->Position[0], Node->Position[1], Node->Position[2] };

	//HF_FontDrawText(Font, Position, Node->Size, "This is a Test!");
	HF_FontDrawHex(Font, Position, Node->Size, Node->Buffer + Node->BufferOffset, Node->BufferSize, 16, 0.4F);

	Node->Size[1] = -Node->Size[1];
}