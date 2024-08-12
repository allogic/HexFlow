#ifndef HF_NODEGRAPH_GRAPH_H
#define HF_NODEGRAPH_GRAPH_H

typedef float HF_Vector2[2];
typedef float HF_Vector3[3];

struct HF_Graph* HF_GraphAlloc(char const *FileName);
void HF_GraphFree(struct HF_Graph *Graph);

void HF_GraphUpdate(struct HF_Graph *Graph);

void HF_GraphDrawLines(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos);
void HF_GraphDrawQuads(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos);
void HF_GraphDrawLineBatch(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos);
void HF_GraphDrawQuadBatch(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos);
void HF_GraphDrawFont(struct HF_Graph *Graph, struct HF_Font *Font);

void HF_GraphAddNode(struct HF_Graph *Graph, HF_Vector3 Position, HF_Vector2 Size, char const *Title, long long unsigned BufferIndex, long long unsigned BufferSize);

#endif // HF_NODEGRAPH_GRAPH_H