#ifndef HF_NODE_GRAPH_GRAPH_H
#define HF_NODE_GRAPH_GRAPH_H

#include <HexFlow/Math/MathTypes.h>

#include <HexFlow/NodeGraph/NodeTypes.h>

struct HF_Graph* HF_GraphAlloc(char const *FileName);
void HF_GraphFree(struct HF_Graph *Graph);

struct HF_TextEditorNode* HF_GraphGetTextEditor(struct HF_Graph *Graph);

void HF_GraphUpdate(struct HF_Graph *Graph);

void HF_GraphDrawFont(struct HF_Graph *Graph, struct HF_Font *Font);
void HF_GraphDrawLines(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos);
void HF_GraphDrawQuads(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos);
void HF_GraphDrawLineBatch(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos);
void HF_GraphDrawQuadBatch(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos);

struct HF_Node* HF_GraphAddNode(struct HF_Graph *Graph, HF_NodeType NodeType, HF_Vector3 Position, HF_Vector2 Size, char const *Title);

#endif // HF_NODE_GRAPH_GRAPH_H