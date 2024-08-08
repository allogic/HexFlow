#ifndef HF_GRAPH_H
#define HF_GRAPH_H

#include <HexFlow/List.h>

struct HF_Graph* HF_GraphAlloc(char const *FileName);
void HF_GraphFree(struct HF_Graph *Graph);

HF_ListEntry* HF_GraphListEntry(struct HF_Graph *Graph);

void HF_GraphDraw(struct HF_Graph *Graph, struct HF_Font *Font);

#endif