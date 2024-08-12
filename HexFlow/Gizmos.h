#ifndef HF_GIZMOS_H
#define HF_GIZMOS_H

typedef float HF_Vector2[2];
typedef float HF_Vector3[3];

typedef float HF_Matrix4[4][4];

struct HF_Gizmos* HF_GizmosAlloc();
void HF_GizmosFree(struct HF_Gizmos *Gizmos);

void HF_GizmosBeginLines(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View, HF_Matrix4 Model);
void HF_GizmosDrawLine(struct HF_Gizmos *Gizmos, HF_Vector3 From, HF_Vector3 To, int unsigned Color);
void HF_GizmosEndLines(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader);

void HF_GizmosBeginQuads(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View, HF_Matrix4 Model);
void HF_GizmosDrawQuad(struct HF_Gizmos *Gizmos, HF_Vector3 Position, HF_Vector2 Size, int unsigned Color);
void HF_GizmosEndQuads(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader);

void HF_GizmosBeginLineBatch(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View);
void HF_GizmosDrawBatchLine(struct HF_Gizmos *Gizmos, HF_Vector3 From, HF_Vector3 To, int unsigned Color);
void HF_GizmosDrawBatchLineRect(struct HF_Gizmos *Gizmos, HF_Vector3 Position, HF_Vector2 Size, int unsigned Color);
void HF_GizmosDrawLineBatch(struct HF_Gizmos *Gizmos);
void HF_GizmosEndLineBatch(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader);

void HF_GizmosBeginQuadBatch(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View);
void HF_GizmosDrawBatchQuad(struct HF_Gizmos *Gizmos, HF_Vector3 Position, HF_Vector2 Size, int unsigned Color);
void HF_GizmosDrawQuadBatch(struct HF_Gizmos *Gizmos);
void HF_GizmosEndQuadBatch(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader);

#endif // HF_GIZMOS_H