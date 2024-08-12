#version 400 core

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

layout (location = 0) in vec3 InputVertexPosition;
layout (location = 1) in uint InputVertexColor;

flat out uint Color;

void main()
{
	Color = InputVertexColor;

	gl_Position = Projection * View * Model * vec4(InputVertexPosition, 1);
}