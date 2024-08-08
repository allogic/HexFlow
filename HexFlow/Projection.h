#ifndef HF_ORTHOGRAPHIC_H
#define HF_ORTHOGRAPHIC_H

typedef float HF_Matrix4[4][4];

void HF_ProjectionOrthographic(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ, HF_Matrix4 R);

#endif