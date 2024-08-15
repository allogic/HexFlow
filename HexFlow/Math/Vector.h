#ifndef HF_MATH_VECTOR_H
#define HF_MATH_VECTOR_H

#include <HexFlow/Math/MathTypes.h>

#define HF_VECTOR2_ZERO { 0.0F, 0.0F }
#define HF_VECTOR3_ZERO { 0.0F, 0.0F, 0.0F }
#define HF_VECTOR4_ZERO { 0.0F, 0.0F, 0.0F, 0.0F }

void HF_Vector2Init(HF_Vector2 R, float X, float Y);
void HF_Vector3Init(HF_Vector3 R, float X, float Y, float Z);
void HF_Vector4Init(HF_Vector4 R, float X, float Y, float Z, float W);

void HF_Vector2Set(HF_Vector2 V, HF_Vector2 R);
void HF_Vector3Set(HF_Vector3 V, HF_Vector3 R);
void HF_Vector4Set(HF_Vector4 V, HF_Vector4 R);

void HF_Vector2Add(HF_Vector2 A, HF_Vector2 B, HF_Vector2 R);
void HF_Vector3Add(HF_Vector3 A, HF_Vector3 B, HF_Vector3 R);
void HF_Vector4Add(HF_Vector4 A, HF_Vector4 B, HF_Vector4 R);

void HF_Vector2Sub(HF_Vector2 A, HF_Vector2 B, HF_Vector2 R);
void HF_Vector3Sub(HF_Vector3 A, HF_Vector3 B, HF_Vector3 R);
void HF_Vector4Sub(HF_Vector4 A, HF_Vector4 B, HF_Vector4 R);

void HF_Vector2DivScalar(HF_Vector2 V, float S, HF_Vector2 R);
void HF_Vector3DivScalar(HF_Vector3 V, float S, HF_Vector3 R);
void HF_Vector4DivScalar(HF_Vector4 V, float S, HF_Vector4 R);

void HF_Vector3MulAdd(HF_Vector3 V, float S, HF_Vector3 R);
void HF_Vector3Scale(HF_Vector3 V, float S, HF_Vector3 R);

void HF_Vector3Norm(HF_Vector3 V);

float HF_Vector3Dot(HF_Vector3 A, HF_Vector3 B);
void HF_Vector3Cross(HF_Vector3 A, HF_Vector3 B, HF_Vector3 R);

#endif // HF_MATH_VECTOR_H