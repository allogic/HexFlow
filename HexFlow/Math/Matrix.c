#include <HexFlow/Math/Vector.h>
#include <HexFlow/Math/Matrix.h>

void HF_Matrix4GetPosition(HF_Matrix4 M, HF_Vector3 P)
{
	P[0] = M[0][0];
	P[1] = M[1][0];
	P[2] = M[2][0];
}

void HF_Matrix4GetScale(HF_Matrix4 M, HF_Vector3 S)
{
	S[0] = M[0][0];
	S[1] = M[1][1];
	S[2] = M[2][2];
}

void HF_Matrix4SetPosition(HF_Matrix4 M, HF_Vector3 P)
{
	M[3][0] = P[0];
	M[3][1] = P[1];
	M[3][2] = P[2];
}

void HF_Matrix4SetScale(HF_Matrix4 M, HF_Vector3 S)
{
	M[0][0] = S[0];
	M[1][1] = S[1];
	M[2][2] = S[2];
}

void HF_Matrix4Zero(HF_Matrix4 M)
{
	M[0][0] = M[0][1] = M[0][2] = M[0][3] = 0.0F;
	M[1][0] = M[1][1] = M[1][2] = M[1][3] = 0.0F;
	M[2][0] = M[2][1] = M[2][2] = M[2][3] = 0.0F;
	M[3][0] = M[3][1] = M[3][2] = M[3][3] = 0.0F;
}

void HF_Matrix4One(HF_Matrix4 M)
{
	M[0][0] = M[0][1] = M[0][2] = M[0][3] = 1.0F;
	M[1][0] = M[1][1] = M[1][2] = M[1][3] = 1.0F;
	M[2][0] = M[2][1] = M[2][2] = M[2][3] = 1.0F;
	M[3][0] = M[3][1] = M[3][2] = M[3][3] = 1.0F;
}

void HF_Matrix4Identity(HF_Matrix4 M)
{
	M[0][0] = 1.0F; M[0][1] = 0.0F; M[0][2] = 0.0F; M[0][3] = 0.0F;
	M[1][0] = 0.0F; M[1][1] = 1.0F; M[1][2] = 0.0F; M[1][3] = 0.0F;
	M[2][0] = 0.0F; M[2][1] = 0.0F; M[2][2] = 1.0F; M[2][3] = 0.0F;
	M[3][0] = 0.0F; M[3][1] = 0.0F; M[3][2] = 0.0F; M[3][3] = 1.0F;
}

void HF_Matrix4Copy(HF_Matrix4 M, HF_Matrix4 R)
{
	R[0][0] = M[0][0]; R[1][0] = M[1][0];
	R[0][1] = M[0][1]; R[1][1] = M[1][1];
	R[0][2] = M[0][2]; R[1][2] = M[1][2];
	R[0][3] = M[0][3]; R[1][3] = M[1][3];

	R[2][0] = M[2][0]; R[3][0] = M[3][0];
	R[2][1] = M[2][1]; R[3][1] = M[3][1];
	R[2][2] = M[2][2]; R[3][2] = M[3][2];
	R[2][3] = M[2][3]; R[3][3] = M[3][3];
}

void HF_Matrix4Mul(HF_Matrix4 A, HF_Matrix4 B, HF_Matrix4 R)
{
	float A00 = A[0][0], A01 = A[0][1], A02 = A[0][2], A03 = A[0][3];
	float A10 = A[1][0], A11 = A[1][1], A12 = A[1][2], A13 = A[1][3];
	float A20 = A[2][0], A21 = A[2][1], A22 = A[2][2], A23 = A[2][3];
	float A30 = A[3][0], A31 = A[3][1], A32 = A[3][2], A33 = A[3][3];

	float B00 = B[0][0], B01 = B[0][1], B02 = B[0][2], B03 = B[0][3];
	float B10 = B[1][0], B11 = B[1][1], B12 = B[1][2], B13 = B[1][3];
	float B20 = B[2][0], B21 = B[2][1], B22 = B[2][2], B23 = B[2][3];
	float B30 = B[3][0], B31 = B[3][1], B32 = B[3][2], B33 = B[3][3];

	R[0][0] = A00 * B00 + A10 * B01 + A20 * B02 + A30 * B03;
	R[0][1] = A01 * B00 + A11 * B01 + A21 * B02 + A31 * B03;
	R[0][2] = A02 * B00 + A12 * B01 + A22 * B02 + A32 * B03;
	R[0][3] = A03 * B00 + A13 * B01 + A23 * B02 + A33 * B03;
	R[1][0] = A00 * B10 + A10 * B11 + A20 * B12 + A30 * B13;
	R[1][1] = A01 * B10 + A11 * B11 + A21 * B12 + A31 * B13;
	R[1][2] = A02 * B10 + A12 * B11 + A22 * B12 + A32 * B13;
	R[1][3] = A03 * B10 + A13 * B11 + A23 * B12 + A33 * B13;
	R[2][0] = A00 * B20 + A10 * B21 + A20 * B22 + A30 * B23;
	R[2][1] = A01 * B20 + A11 * B21 + A21 * B22 + A31 * B23;
	R[2][2] = A02 * B20 + A12 * B21 + A22 * B22 + A32 * B23;
	R[2][3] = A03 * B20 + A13 * B21 + A23 * B22 + A33 * B23;
	R[3][0] = A00 * B30 + A10 * B31 + A20 * B32 + A30 * B33;
	R[3][1] = A01 * B30 + A11 * B31 + A21 * B32 + A31 * B33;
	R[3][2] = A02 * B30 + A12 * B31 + A22 * B32 + A32 * B33;
	R[3][3] = A03 * B30 + A13 * B31 + A23 * B32 + A33 * B33;
}

void HF_Matrix4Translate(HF_Matrix4 M, HF_Vector3 P)
{
	HF_Vector3MulAdd(M[0], P[0], M[3]);
	HF_Vector3MulAdd(M[1], P[1], M[3]);
	HF_Vector3MulAdd(M[2], P[2], M[3]);
}