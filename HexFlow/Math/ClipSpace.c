#include <math.h>

#include <HexFlow/Math/Matrix.h>
#include <HexFlow/Math/ClipSpace.h>

void HF_ClipSpaceOrthographic(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ, HF_Matrix4 R)
{
	HF_Matrix4Identity(R);

	R[0][0] = 2.0F / (Right - Left);
	R[1][1] = 2.0F / (Top - Bottom);
	R[2][2] = 1.0F / (FarZ - NearZ);
	R[3][0] = -(Right + Left) / (Right - Left);
	R[3][1] = -(Top + Bottom) / (Top - Bottom);
	R[3][2] = -NearZ / (FarZ - NearZ);
}

void HF_ClipSpacePerspective(float FieldOfView, float AspectRatio, float NearZ, float FarZ, HF_Matrix4 R)
{
	float TanHalfFieldOfView = tanf(FieldOfView / 2.0F);

	HF_Matrix4Identity(R);

	R[0][0] = 1.0F / (AspectRatio * TanHalfFieldOfView);
	R[1][1] = 1.0F / TanHalfFieldOfView;
	R[2][2] = FarZ / (FarZ - NearZ);
	R[2][3] = 1.0F;
	R[3][2] = -(FarZ * NearZ) / (FarZ - NearZ);
}