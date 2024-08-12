#version 400 core

flat in uint Color;

out vec4 OutputColor;

void main()
{
	float R = float((Color >> 24) & 0xFF) / 255;
	float G = float((Color >> 16) & 0xFF) / 255;
	float B = float((Color >>  8) & 0xFF) / 255;
	float A = float((Color >>  0) & 0xFF) / 255;

	OutputColor = vec4(R, G, B, A);
}