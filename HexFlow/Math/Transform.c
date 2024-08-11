#include <math.h>

#include <HexFlow/Math/Matrix.h>
#include <HexFlow/Math/Transform.h>
#include <HexFlow/Math/Vector.h>

void HF_TransformLookAt(HF_Vector3 Eye, HF_Vector3 Center, HF_Vector3 Up, HF_Matrix4 R)
{
	HF_Vector3 F, S, U;

	HF_Vector3Sub(Center, Eye, F);
	HF_Vector3Norm(F);

	HF_Vector3Cross(Up, F, S);
	HF_Vector3Norm(S);

	HF_Vector3Cross(F, S, U);

	HF_Matrix4Identity(R);

	R[0][0] = S[0];
	R[1][0] = S[1];
	R[2][0] = S[2];
	R[0][1] = U[0];
	R[1][1] = U[1];
	R[2][1] = U[2];
	R[0][2] = F[0];
	R[1][2] = F[1];
	R[2][2] = F[2];
	R[3][0] = -HF_Vector3Dot(S, Eye);
	R[3][1] = -HF_Vector3Dot(U, Eye);
	R[3][2] = -HF_Vector3Dot(F, Eye);
}