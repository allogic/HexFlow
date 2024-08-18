#ifndef HF_NODE_GRAPH_NODE_H
#define HF_NODE_GRAPH_NODE_H

#include <HexFlow/Math/MathTypes.h>

#include <HexFlow/NodeGraph/NodeTypes.h>

struct HF_Node* HF_NodeAlloc(HF_NodeType NodeType, char const *Title, HF_Vector3 Position, HF_Vector2 Size);
void HF_NodeFree(struct HF_Node *Node);

HF_NodeType HF_NodeGetType(struct HF_Node *Node);

void HF_NodeGetPosition(struct HF_Node *Node, HF_Vector3 Position);
void HF_NodeGetSize(struct HF_Node *Node, HF_Vector2 Size);

void HF_NodeGetContentPosition(struct HF_Node *Node, HF_Vector3 Position);
void HF_NodeGetContentSize(struct HF_Node *Node, HF_Vector2 Size);

void* HF_NodeGetVirtualNode(struct HF_Node *Node);

void HF_NodeUpdate(struct HF_Node *Node);

void HF_NodeDrawFont(struct HF_Node *Node, struct HF_Font *Font);
void HF_NodeDrawLines(struct HF_Node *Node, struct HF_Gizmos *Gizmos);
void HF_NodeDrawQuads(struct HF_Node *Node, struct HF_Gizmos *Gizmos);
void HF_NodeDrawLineBatch(struct HF_Node *Node, struct HF_Gizmos *Gizmos);
void HF_NodeDrawQuadBatch(struct HF_Node *Node, struct HF_Gizmos *Gizmos);

#endif // HF_NODE_GRAPH_NODE_H