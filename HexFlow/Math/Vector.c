#include <math.h>

#include <HexFlow/Math/Vector.h>

void HF_Vector2Set(HF_Vector2 V, HF_Vector2 R)
{
	R[0] = V[0];
	R[1] = V[1];
}

void HF_Vector3Set(HF_Vector3 V, HF_Vector3 R)
{
	R[0] = V[0];
	R[1] = V[1];
	R[2] = V[2];
}

void HF_Vector4Set(HF_Vector4 V, HF_Vector4 R)
{
	R[0] = V[0];
	R[1] = V[1];
	R[2] = V[2];
	R[3] = V[3];
}

void HF_Vector2Add(HF_Vector2 A, HF_Vector2 B, HF_Vector2 R)
{
	R[0] = A[0] + B[0];
	R[1] = A[1] + B[1];
}

void HF_Vector3Add(HF_Vector3 A, HF_Vector3 B, HF_Vector3 R)
{
	R[0] = A[0] + B[0];
	R[1] = A[1] + B[1];
	R[2] = A[2] + B[2];
}

void HF_Vector4Add(HF_Vector4 A, HF_Vector4 B, HF_Vector4 R)
{
	R[0] = A[0] + B[0];
	R[1] = A[1] + B[1];
	R[2] = A[2] + B[2];
	R[3] = A[3] + B[3];
}

void HF_Vector2DivScalar(HF_Vector2 V, float S, HF_Vector2 R)
{
	R[0] = V[0] / S;
	R[1] = V[1] / S;
}

void HF_Vector3DivScalar(HF_Vector3 V, float S, HF_Vector3 R)
{
	R[0] = V[0] / S;
	R[1] = V[1] / S;
	R[2] = V[2] / S;
}

void HF_Vector4DivScalar(HF_Vector4 V, float S, HF_Vector4 R)
{
	R[0] = V[0] / S;
	R[1] = V[1] / S;
	R[2] = V[2] / S;
	R[3] = V[3] / S;
}

void HF_Vector3Sub(HF_Vector3 A, HF_Vector3 B, HF_Vector3 R)
{
	R[0] = A[0] - B[0];
	R[1] = A[1] - B[1];
	R[2] = A[2] - B[2];
}

void HF_Vector3MulAdd(HF_Vector3 V, float S, HF_Vector3 R)
{
	R[0] += V[0] * S;
	R[1] += V[1] * S;
	R[2] += V[2] * S;
}

void HF_Vector3Scale(HF_Vector3 V, float S, HF_Vector3 R)
{
	R[0] = V[0] * S;
	R[1] = V[1] * S;
	R[2] = V[2] * S;
}

void HF_Vector3Norm(HF_Vector3 V)
{
	float N = sqrtf(HF_Vector3Dot(V, V));

	if (N == 0.0F)
	{
		V[0] = V[1] = V[2] = 0.0F;
	}
	else
	{
		HF_Vector3Scale(V, 1.0F / N, V);
	}
}

float HF_Vector3Dot(HF_Vector3 A, HF_Vector3 B)
{
	return A[0] * B[0] + A[1] * B[1] + A[2] * B[2];
}

void HF_Vector3Cross(HF_Vector3 A, HF_Vector3 B, HF_Vector3 R)
{
	R[0] = A[1] * B[2] - A[2] * B[1];
	R[1] = A[2] * B[0] - A[0] * B[2];
	R[2] = A[0] * B[1] - A[1] * B[0];
}