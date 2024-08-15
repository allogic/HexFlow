#include <string.h>

#include <HexFlow/Font.h>
#include <HexFlow/Gizmos.h>
#include <HexFlow/Memory.h>

#include <HexFlow/Math/Vector.h>

#include <HexFlow/NodeGraph/Node.h>

#include <HexFlow/NodeGraph/Nodes/TextEditorNode.h>

struct HF_TextEditorNode
{
	struct HF_Node *Node;

	int unsigned Row;
	int unsigned Column;
};

struct HF_TextEditorNode* HF_TextEditorNodeAlloc(struct HF_Node *Node)
{
	struct HF_TextEditorNode *TextEditorNode = (struct HF_TextEditorNode*)HF_MemoryAlloc(sizeof(struct HF_TextEditorNode), 0);

	TextEditorNode->Node = Node;

	TextEditorNode->Row = 0;
	TextEditorNode->Column = 0;

	return TextEditorNode;
}

void HF_TextEditorNodeFree(struct HF_TextEditorNode *TextEditorNode)
{
	char unsigned *Buffer = HF_NodeGetBuffer(TextEditorNode->Node);

	HF_MemoryFree(Buffer);
	HF_MemoryFree(TextEditorNode);
}

void HF_TextEditorNodeKeyCallback(struct HF_TextEditorNode *TextEditorNode, int Key, int ScanCode, int Action, int Mods)
{
	char unsigned *Buffer = HF_NodeGetBuffer(TextEditorNode->Node);

	//Buffer[TextEditorNode->Row] // TODO
}

void HF_TextEditorNodeUpdate(struct HF_TextEditorNode *TextEditorNode)
{

}

void HF_TextEditorNodeDrawFont(struct HF_TextEditorNode *TextEditorNode, struct HF_Font *Font)
{
	HF_Vector2 ContentSize;

	HF_NodeGetContentSize(TextEditorNode->Node, ContentSize);

	HF_Vector3 ContentPosition;

	HF_NodeGetContentPosition(TextEditorNode->Node, ContentPosition);

	char unsigned *Buffer = HF_NodeGetBuffer(TextEditorNode->Node);

	HF_FontDrawTextClipped(Font, ContentPosition, ContentSize, Buffer);
}

void HF_TextEditorNodeDrawLines(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos)
{

}

void HF_TextEditorNodeDrawQuads(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos)
{

}

void HF_TextEditorNodeDrawLineBatch(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos)
{

}

void HF_TextEditorNodeDrawQuadBatch(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos)
{

}