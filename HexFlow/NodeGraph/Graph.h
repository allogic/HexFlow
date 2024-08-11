#ifndef HF_NODEGRAPH_GRAPH_H
#define HF_NODEGRAPH_GRAPH_H

struct HF_Graph* HF_GraphAlloc(char const *FileName);
void HF_GraphFree(struct HF_Graph *Graph);

void HF_GraphUpdate(struct HF_Graph *Graph);

void HF_GraphDrawGizmos(struct HF_Graph *Graph, struct HF_Gizmos *Gizmos);
void HF_GraphDrawFont(struct HF_Graph *Graph, struct HF_Font *Font);

void HF_GraphAddNode(struct HF_Graph *Graph, long long unsigned Index, long long unsigned Size);

#endif // HF_NODEGRAPH_GRAPH_H