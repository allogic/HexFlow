#version 400 core

struct HF_Glyph
{
	uint Start, Count;
};

struct HF_Curve
{
	vec2 P0, P1, P2;
};

uniform usamplerBuffer Glyphs;
uniform samplerBuffer Curves;
uniform vec4 Color;
uniform float AntiAliasingWindowSize = 1.0;
uniform bool EnableSuperSamplingAntiAliasing = false;
uniform bool EnableControlPointsVisualization = false;

in vec2 UV;
flat in uint BufferIndex;

out vec4 OutputColor;

vec2 HF_Rotate(vec2 V)
{
	return vec2(V.y, -V.x);
}

HF_Glyph HF_LoadGlyph(uint Index)
{
	HF_Glyph Glyph;

	uvec2 Data = texelFetch(Glyphs, int(Index)).xy;

	Glyph.Start = Data.x;
	Glyph.Count = Data.y;

	return Glyph;
}

HF_Curve HF_LoadCurve(uint Index)
{
	HF_Curve Curve;

	Curve.P0 = texelFetch(Curves, int(3 * Index + 0)).xy;
	Curve.P1 = texelFetch(Curves, int(3 * Index + 1)).xy;
	Curve.P2 = texelFetch(Curves, int(3 * Index + 2)).xy;

	return Curve;
}

float HF_ComputeCoverage(float InverseDiameter, vec2 P0, vec2 P1, vec2 P2)
{
	if (P0.y > 0 && P1.y > 0 && P2.y > 0) return 0;
	if (P0.y < 0 && P1.y < 0 && P2.y < 0) return 0;

	vec2 A = P0 - 2 * P1 + P2;
	vec2 B = P0 - P1;
	vec2 C = P0;

	float T0, T1;

	if (abs(A.y) >= 1e-5)
	{
		float Radicand = B.y * B.y - A.y * C.y;

		if (Radicand <= 0) return 0;
	
		float S = sqrt(Radicand);

		T0 = (B.y - S) / A.y;
		T1 = (B.y + S) / A.y;
	}
	else
	{
		float T = P0.y / (P0.y - P2.y);

		if (P0.y < P2.y)
		{
			T0 = -1.0;
			T1 = T;
		}
		else
		{
			T0 = T;
			T1 = -1.0;
		}
	}

	float Alpha = 0;
	
	if (T0 >= 0 && T0 < 1)
	{
		float X = (A.x * T0 - 2 * B.x) * T0 + C.x;

		Alpha += clamp(X * InverseDiameter + 0.5, 0, 1);
	}

	if (T1 >= 0 && T1 < 1)
	{
		float X = (A.x * T1 - 2 * B.x) * T1 + C.x;

		Alpha -= clamp(X * InverseDiameter + 0.5, 0, 1);
	}

	return Alpha;
}

void main()
{
	float Alpha = 0;

	vec2 InverseDiameter = 1 / (AntiAliasingWindowSize * fwidth(UV));

	HF_Glyph Glyph = HF_LoadGlyph(BufferIndex);

	for (uint CurveIndex = 0; CurveIndex < Glyph.Count; CurveIndex++)
	{
		HF_Curve Curve = HF_LoadCurve(Glyph.Start + CurveIndex);

		vec2 P0 = Curve.P0 - UV;
		vec2 P1 = Curve.P1 - UV;
		vec2 P2 = Curve.P2 - UV;

		Alpha += HF_ComputeCoverage(InverseDiameter.x, P0, P1, P2);

		if (EnableSuperSamplingAntiAliasing)
		{
			Alpha += HF_ComputeCoverage(InverseDiameter.y, HF_Rotate(P0), HF_Rotate(P1), HF_Rotate(P2));
		}
	}

	if (EnableSuperSamplingAntiAliasing)
	{
		Alpha *= 0.5;
	}

	Alpha = clamp(Alpha, 0, 1);

	OutputColor = Color * Alpha;

	if (EnableControlPointsVisualization)
	{
		vec2 FW = fwidth(UV);

		float R = 4 * 0.5 * (FW.x + FW.y);

		for (uint CurveIndex = 0; CurveIndex < Glyph.Count; CurveIndex++)
		{
			HF_Curve Curve = HF_LoadCurve(Glyph.Start + CurveIndex);

			vec2 P0 = Curve.P0 - UV;
			vec2 P1 = Curve.P1 - UV;
			vec2 P2 = Curve.P2 - UV;

			if (dot(P0, P0) < R * R || dot(P2, P2) < R * R)
			{
				OutputColor = vec4(0, 1, 0, 1);
			}

			if (dot(P1, P1) < R * R)
			{
				OutputColor = vec4(1, 0, 1, 1);
			}
		}
	}
}