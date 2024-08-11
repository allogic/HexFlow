#include <stdlib.h>

#include <HexFlow/Gizmos.h>
#include <HexFlow/Shader.h>
#include <HexFlow/Memory.h>

#include <HexFlow/GLAD/glad.h>

struct HF_BufferVertex
{
	HF_Vector3 Position;
	HF_Vector2 TextureCoords;

	int unsigned Color;
};

struct HF_Gizmos
{
	int unsigned Vao, Vbo, Ebo;
};

struct HF_Gizmos* HF_GizmosAlloc()
{
	struct HF_Gizmos *Gizmos = (struct HF_Gizmos*)HF_MemoryAlloc(sizeof(struct HF_Gizmos), 0);

	glGenVertexArrays(1, &Gizmos->Vao);

	glGenBuffers(1, &Gizmos->Vbo);
	glGenBuffers(1, &Gizmos->Ebo);

	glBindVertexArray(Gizmos->Vao);
	glBindBuffer(GL_ARRAY_BUFFER, Gizmos->Vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Gizmos->Ebo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(struct HF_BufferVertex), (void*)(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(struct HF_BufferVertex), (void*)(sizeof(HF_Vector3)));
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(struct HF_BufferVertex), (void*)(sizeof(HF_Vector3) + sizeof(HF_Vector2)));
	glBindVertexArray(0);

	return Gizmos;
}

void HF_GizmosFree(struct HF_Gizmos *Gizmos)
{
	glDeleteVertexArrays(1, &Gizmos->Vao);

	glDeleteBuffers(1, &Gizmos->Vbo);
	glDeleteBuffers(1, &Gizmos->Ebo);

	HF_MemoryFree(Gizmos);
}

void HF_GizmosBeginDraw(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View, HF_Matrix4 Model)
{
	HF_ShaderBind(Shader);

	HF_ShaderSet1Matrix4(Shader, "Projection", Projection);
	HF_ShaderSet1Matrix4(Shader, "View", View);
	HF_ShaderSet1Matrix4(Shader, "Model", Model);
}

void HF_GizmosDrawQuad(struct HF_Gizmos *Gizmos, HF_Vector3 Position, HF_Vector2 Size, int unsigned Color)
{
	static struct HF_BufferVertex VertexBuffer[4];
	static int unsigned IndexBuffer[6];

	glBindVertexArray(Gizmos->Vao);

	VertexBuffer[0].Position[0] = Position[0];
	VertexBuffer[0].Position[1] = Position[1];
	VertexBuffer[0].Position[2] = Position[2];
	VertexBuffer[0].TextureCoords[0] = 0.0F;
	VertexBuffer[0].TextureCoords[1] = 0.0F;
	VertexBuffer[0].Color = Color;

	VertexBuffer[1].Position[0] = Position[0] + Size[0];
	VertexBuffer[1].Position[1] = Position[1];
	VertexBuffer[1].Position[2] = Position[2];
	VertexBuffer[1].TextureCoords[0] = 1.0F;
	VertexBuffer[1].TextureCoords[1] = 0.0F;
	VertexBuffer[1].Color = Color;

	VertexBuffer[2].Position[0] = Position[0] + Size[0];
	VertexBuffer[2].Position[1] = Position[1] - Size[1];
	VertexBuffer[2].Position[2] = Position[2];
	VertexBuffer[2].TextureCoords[0] = 1.0F;
	VertexBuffer[2].TextureCoords[1] = 1.0F;
	VertexBuffer[2].Color = Color;

	VertexBuffer[3].Position[0] = Position[0];
	VertexBuffer[3].Position[1] = Position[1] - Size[1];
	VertexBuffer[3].Position[2] = Position[2];
	VertexBuffer[3].TextureCoords[0] = 0.0F;
	VertexBuffer[3].TextureCoords[1] = 1.0F;
	VertexBuffer[3].Color = Color;

	IndexBuffer[0] = 0;
	IndexBuffer[1] = 2;
	IndexBuffer[2] = 1;

	IndexBuffer[3] = 2;
	IndexBuffer[4] = 0;
	IndexBuffer[5] = 3;

	glBindBuffer(GL_ARRAY_BUFFER, Gizmos->Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct HF_BufferVertex) * 4, VertexBuffer, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Gizmos->Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int unsigned) * 6, IndexBuffer, GL_STREAM_DRAW);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void HF_GizmosEndDraw(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader)
{
	HF_ShaderUnbind(Shader);
}