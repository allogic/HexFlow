#ifndef HF_MATH_MATRIX_H
#define HF_MATH_MATRIX_H

#include <HexFlow/Math/MathTypes.h>

#define HF_MATRIX4_ZERO { { 0.0F, 0.0F, 0.0F, 0.0F }, \
                          { 0.0F, 0.0F, 0.0F, 0.0F }, \
                          { 0.0F, 0.0F, 0.0F, 0.0F }, \
                          { 0.0F, 0.0F, 0.0F, 0.0F } }

#define HF_MATRIX4_IDENTITY { { 1.0F, 0.0F, 0.0F, 0.0F }, \
                              { 0.0F, 1.0F, 0.0F, 0.0F }, \
                              { 0.0F, 0.0F, 1.0F, 0.0F }, \
                              { 0.0F, 0.0F, 0.0F, 1.0F } }

void HF_Matrix4GetPosition(HF_Matrix4 M, HF_Vector3 P);
void HF_Matrix4GetScale(HF_Matrix4 M, HF_Vector3 S);

void HF_Matrix4SetPosition(HF_Matrix4 M, HF_Vector3 P);
void HF_Matrix4SetScale(HF_Matrix4 M, HF_Vector3 S);

void HF_Matrix4Zero(HF_Matrix4 M);
void HF_Matrix4One(HF_Matrix4 M);

void HF_Matrix4Identity(HF_Matrix4 M);
void HF_Matrix4Copy(HF_Matrix4 M, HF_Matrix4 R);

void HF_Matrix4Mul(HF_Matrix4 A, HF_Matrix4 B, HF_Matrix4 R);

void HF_Matrix4Translate(HF_Matrix4 M, HF_Vector3 P);

#endif // HF_MATH_MATRIX_H