#ifndef HF_MATH_TRANSFORM_H
#define HF_MATH_TRANSFORM_H

typedef float HF_Vector3[3];

typedef float HF_Matrix4[4][4];

void HF_TransformLookAt(HF_Vector3 Eye, HF_Vector3 Center, HF_Vector3 Up, HF_Matrix4 R);

#endif // HF_MATH_TRANSFORM_H