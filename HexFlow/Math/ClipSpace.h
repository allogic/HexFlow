#ifndef HF_MATH_CLIPSPACE_H
#define HF_MATH_CLIPSPACE_H

typedef float HF_Matrix4[4][4];

void HF_ClipSpaceOrthographic(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ, HF_Matrix4 R);
void HF_ClipSpacePerspective(float FieldOfView, float AspectRatio, float NearZ, float FarZ, HF_Matrix4 R);

#endif // HF_MATH_CLIPSPACE_H