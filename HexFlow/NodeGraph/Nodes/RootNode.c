#include <HexFlow/Font.h>
#include <HexFlow/Gizmos.h>
#include <HexFlow/Memory.h>

#include <HexFlow/NodeGraph/Node.h>

#include <HexFlow/NodeGraph/Nodes/RootNode.h>

struct HF_RootNode
{
	struct HF_Node *Node;
};

struct HF_RootNode* HF_RootNodeAlloc(struct HF_Node *Node)
{
	struct HF_RootNode *RootNode = (struct HF_RootNode*)HF_MemoryAlloc(sizeof(struct HF_RootNode), 0);

	RootNode->Node = Node;

	return RootNode;
}

void HF_RootNodeFree(struct HF_RootNode *RootNode)
{
	HF_MemoryFree(RootNode);
}

void HF_RootNodeUpdate(struct HF_RootNode *RootNode)
{

}

void HF_RootNodeDrawFont(struct HF_RootNode *RootNode, struct HF_Font *Font)
{
	HF_Vector2 ContentSize;

	HF_NodeGetContentSize(RootNode->Node, ContentSize);

	HF_Vector3 ContentPosition;

	HF_NodeGetContentPosition(RootNode->Node, ContentPosition);

	char unsigned *Buffer = HF_NodeGetBuffer(RootNode->Node);

	long long unsigned BufferOffset = HF_NodeGetBufferOffset(RootNode->Node);
	long long unsigned BufferSize = HF_NodeGetBufferSize(RootNode->Node);

	HF_FontDrawHexClipped(Font, ContentPosition, ContentSize, Buffer + BufferOffset, BufferSize, 16, 0.4F);
}

void HF_RootNodeDrawLines(struct HF_RootNode *RootNode, struct HF_Gizmos *Gizmos)
{

}

void HF_RootNodeDrawQuads(struct HF_RootNode *RootNode, struct HF_Gizmos *Gizmos)
{

}

void HF_RootNodeDrawLineBatch(struct HF_RootNode *RootNode, struct HF_Gizmos *Gizmos)
{

}

void HF_RootNodeDrawQuadBatch(struct HF_RootNode *RootNode, struct HF_Gizmos *Gizmos)
{

}