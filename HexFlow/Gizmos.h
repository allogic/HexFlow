#ifndef HF_GIZMOS_H
#define HF_GIZMOS_H

typedef float HF_Vector2[2];
typedef float HF_Vector3[3];

typedef float HF_Matrix4[4][4];

struct HF_Gizmos* HF_GizmosAlloc();
void HF_GizmosFree(struct HF_Gizmos *Gizmos);

void HF_GizmosBeginDraw(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View, HF_Matrix4 Model);

void HF_GizmosDrawQuad(struct HF_Gizmos *Gizmos, HF_Vector3 Position, HF_Vector2 Size, int unsigned Color);

void HF_GizmosEndDraw(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader);

#endif // HF_GIZMOS_H