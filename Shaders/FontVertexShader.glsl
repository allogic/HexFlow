#version 400 core

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

layout (location = 0) in vec2 InputVertexPosition;
layout (location = 1) in vec2 InputVertexUV;
layout (location = 2) in uint InputVertexIndex;

out vec2 UV;
flat out uint BufferIndex;

void main()
{
	UV = InputVertexUV;
	BufferIndex = InputVertexIndex;

	gl_Position = Projection * View * Model * vec4(InputVertexPosition, 0, 1);
}