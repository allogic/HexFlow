#ifndef HF_NODE_GRAPH_NODES_TEXT_EDITOR_NODE_H
#define HF_NODE_GRAPH_NODES_TEXT_EDITOR_NODE_H

#include <HexFlow/Math/MathTypes.h>

struct HF_TextEditorNode* HF_TextEditorNodeAlloc(struct HF_Node *Node);
void HF_TextEditorNodeFree(struct HF_TextEditorNode *TextEditorNode);

void HF_TextEditorNodeKeyCallback(struct HF_TextEditorNode *TextEditorNode, int Key, int ScanCode, int Action, int Mods);

void HF_TextEditorNodeUpdate(struct HF_TextEditorNode *TextEditorNode);

void HF_TextEditorNodeDrawFont(struct HF_TextEditorNode *TextEditorNode, struct HF_Font *Font);
void HF_TextEditorNodeDrawLines(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos);
void HF_TextEditorNodeDrawQuads(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos);
void HF_TextEditorNodeDrawLineBatch(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos);
void HF_TextEditorNodeDrawQuadBatch(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos);

#endif // HF_NODE_GRAPH_NODES_TEXT_EDITOR_NODE_H