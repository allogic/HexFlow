#ifndef HF_NODEGRAPH_NODE_H
#define HF_NODEGRAPH_NODE_H

typedef float HF_Vector2[2];
typedef float HF_Vector3[3];

struct HF_Node* HF_NodeAlloc(char const *Title, HF_Vector3 Position, HF_Vector2 Size, char unsigned *Buffer, long long unsigned BufferOffset, long long unsigned BufferSize);
void HF_NodeFree(struct HF_Node *Node);

void HF_NodeUpdate(struct HF_Node *Node);

void HF_NodeDrawLines(struct HF_Node *Node, struct HF_Gizmos *Gizmos);
void HF_NodeDrawQuads(struct HF_Node *Node, struct HF_Gizmos *Gizmos);
void HF_NodeDrawLineBatch(struct HF_Node *Node, struct HF_Gizmos *Gizmos);
void HF_NodeDrawQuadBatch(struct HF_Node *Node, struct HF_Gizmos *Gizmos);
void HF_NodeDrawFont(struct HF_Node *Node, struct HF_Font *Font);

#endif // HF_NODEGRAPH_NODE_H