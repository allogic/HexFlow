#include <HexFlow/Matrix.h>
#include <HexFlow/Projection.h>

void HF_ProjectionOrthographic(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ, HF_Matrix4 R)
{
	float RL, TB, FN;

	HF_Matrix4Zero(R);

	RL = 1.0F / (Right - Left);
	TB = 1.0F / (Top - Bottom);
	FN = -1.0F / (FarZ - NearZ);

	R[0][0] = 2.0F * RL;
	R[1][1] = 2.0F * TB;
	R[2][2] = -FN;
	R[3][0] = -(Right + Left) * RL;
	R[3][1] = -(Top + Bottom) * TB;
	R[3][2] = NearZ * FN;
	R[3][3] = 1.0F;
}