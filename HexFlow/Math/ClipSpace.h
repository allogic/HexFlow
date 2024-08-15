#ifndef HF_MATH_CLIP_SPACE_H
#define HF_MATH_CLIP_SPACE_H

#include <HexFlow/Math/MathTypes.h>

void HF_ClipSpaceOrthographic(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ, HF_Matrix4 R);
void HF_ClipSpacePerspective(float FieldOfView, float AspectRatio, float NearZ, float FarZ, HF_Matrix4 R);

#endif // HF_MATH_CLIP_SPACE_H