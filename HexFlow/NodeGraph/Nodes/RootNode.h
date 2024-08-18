#ifndef HF_NODE_GRAPH_NODES_ROOT_NODE_H
#define HF_NODE_GRAPH_NODES_ROOT_NODE_H

struct HF_RootNode* HF_RootNodeAlloc(struct HF_Node *Node);
void HF_RootNodeFree(struct HF_RootNode *RootNode);

void HF_RootNodeSetBuffer(struct HF_RootNode *RootNode, char unsigned *Buffer);
void HF_RootNodeSetBufferSize(struct HF_RootNode *RootNode, long long unsigned BufferSize);

void HF_RootNodeUpdate(struct HF_RootNode *RootNode);

void HF_RootNodeDrawFont(struct HF_RootNode *RootNode, struct HF_Font *Font);
void HF_RootNodeDrawLines(struct HF_RootNode *RootNode, struct HF_Gizmos *Gizmos);
void HF_RootNodeDrawQuads(struct HF_RootNode *RootNode, struct HF_Gizmos *Gizmos);
void HF_RootNodeDrawLineBatch(struct HF_RootNode *RootNode, struct HF_Gizmos *Gizmos);
void HF_RootNodeDrawQuadBatch(struct HF_RootNode *RootNode, struct HF_Gizmos *Gizmos);

#endif // HF_NODE_GRAPH_NODES_ROOT_NODE_H