#include <stdlib.h>

#include <HexFlow/Gizmos.h>
#include <HexFlow/Shader.h>
#include <HexFlow/Memory.h>

#include <HexFlow/GLAD/glad.h>

#define HF_LINE_BATCH_VERTEX_BUFFER_COUNT 0x10000
#define HF_LINE_BATCH_INDEX_BUFFER_COUNT 0x20000

#define HF_QUAD_BATCH_VERTEX_BUFFER_COUNT 0x10000
#define HF_QUAD_BATCH_INDEX_BUFFER_COUNT 0x60000

struct HF_BufferLineVertex
{
	HF_Vector3 Position;

	int unsigned Color;
};

struct HF_BufferQuadVertex
{
	HF_Vector3 Position;
	HF_Vector2 TextureCoords;

	int unsigned Color;
};

struct HF_Gizmos
{
	int unsigned LineVao, LineVbo, LineEbo;
	int unsigned QuadVao, QuadVbo, QuadEbo;

	int unsigned LineBatchVao, LineBatchVbo, LineBatchEbo;
	int unsigned QuadBatchVao, QuadBatchVbo, QuadBatchEbo;

	struct HF_BufferLineVertex *VertexLineBatchBuffer;
	struct HF_BufferQuadVertex *VertexQuadBatchBuffer;

	int unsigned *IndexLineBatchBuffer;
	int unsigned *IndexQuadBatchBuffer;

	int unsigned LineBatchVertexOffset;
	int unsigned QuadBatchVertexOffset;

	int unsigned LineBatchIndexOffset;
	int unsigned QuadBatchIndexOffset;
};

struct HF_Gizmos* HF_GizmosAlloc()
{
	struct HF_Gizmos *Gizmos = (struct HF_Gizmos*)HF_MemoryAlloc(sizeof(struct HF_Gizmos), 0);

	glGenVertexArrays(1, &Gizmos->LineVao);
	glGenVertexArrays(1, &Gizmos->QuadVao);
	glGenVertexArrays(1, &Gizmos->LineBatchVao);
	glGenVertexArrays(1, &Gizmos->QuadBatchVao);

	glGenBuffers(1, &Gizmos->LineVbo);
	glGenBuffers(1, &Gizmos->LineEbo);
	glGenBuffers(1, &Gizmos->QuadVbo);
	glGenBuffers(1, &Gizmos->QuadEbo);
	glGenBuffers(1, &Gizmos->LineBatchVbo);
	glGenBuffers(1, &Gizmos->LineBatchEbo);
	glGenBuffers(1, &Gizmos->QuadBatchVbo);
	glGenBuffers(1, &Gizmos->QuadBatchEbo);

	glBindVertexArray(Gizmos->LineVao);
	glBindBuffer(GL_ARRAY_BUFFER, Gizmos->LineVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Gizmos->LineEbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(struct HF_BufferLineVertex), (void*)(0));
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(struct HF_BufferLineVertex), (void*)(sizeof(HF_Vector3)));
	glBindVertexArray(0);

	glBindVertexArray(Gizmos->QuadVao);
	glBindBuffer(GL_ARRAY_BUFFER, Gizmos->QuadVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Gizmos->QuadEbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(struct HF_BufferQuadVertex), (void*)(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(struct HF_BufferQuadVertex), (void*)(sizeof(HF_Vector3)));
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(struct HF_BufferQuadVertex), (void*)(sizeof(HF_Vector3) + sizeof(HF_Vector2)));
	glBindVertexArray(0);

	glBindVertexArray(Gizmos->LineBatchVao);
	glBindBuffer(GL_ARRAY_BUFFER, Gizmos->LineBatchVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Gizmos->LineBatchEbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(struct HF_BufferLineVertex), (void*)(0));
	glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, sizeof(struct HF_BufferLineVertex), (void*)(sizeof(HF_Vector3)));
	glBindVertexArray(0);

	glBindVertexArray(Gizmos->QuadBatchVao);
	glBindBuffer(GL_ARRAY_BUFFER, Gizmos->QuadBatchVbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Gizmos->QuadBatchEbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(struct HF_BufferQuadVertex), (void*)(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(struct HF_BufferQuadVertex), (void*)(sizeof(HF_Vector3)));
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(struct HF_BufferQuadVertex), (void*)(sizeof(HF_Vector3) + sizeof(HF_Vector2)));
	glBindVertexArray(0);

	Gizmos->VertexLineBatchBuffer = (struct HF_BufferLineVertex*)HF_MemoryAlloc(HF_LINE_BATCH_VERTEX_BUFFER_COUNT, 0);
	Gizmos->VertexQuadBatchBuffer = (struct HF_BufferQuadVertex*)HF_MemoryAlloc(HF_QUAD_BATCH_VERTEX_BUFFER_COUNT, 0);

	Gizmos->IndexLineBatchBuffer = (int unsigned*)HF_MemoryAlloc(HF_LINE_BATCH_INDEX_BUFFER_COUNT, 0);
	Gizmos->IndexQuadBatchBuffer = (int unsigned*)HF_MemoryAlloc(HF_QUAD_BATCH_INDEX_BUFFER_COUNT, 0);

	return Gizmos;
}

void HF_GizmosFree(struct HF_Gizmos *Gizmos)
{
	glDeleteVertexArrays(1, &Gizmos->LineVao);
	glDeleteVertexArrays(1, &Gizmos->QuadVao);
	glDeleteVertexArrays(1, &Gizmos->LineBatchVao);
	glDeleteVertexArrays(1, &Gizmos->QuadBatchVao);

	glDeleteBuffers(1, &Gizmos->LineVbo);
	glDeleteBuffers(1, &Gizmos->LineEbo);
	glDeleteBuffers(1, &Gizmos->QuadVbo);
	glDeleteBuffers(1, &Gizmos->QuadEbo);
	glDeleteBuffers(1, &Gizmos->LineBatchVbo);
	glDeleteBuffers(1, &Gizmos->LineBatchEbo);
	glDeleteBuffers(1, &Gizmos->QuadBatchVbo);
	glDeleteBuffers(1, &Gizmos->QuadBatchEbo);

	HF_MemoryFree(Gizmos->VertexLineBatchBuffer);
	HF_MemoryFree(Gizmos->VertexQuadBatchBuffer);

	HF_MemoryFree(Gizmos->IndexLineBatchBuffer);
	HF_MemoryFree(Gizmos->IndexQuadBatchBuffer);

	HF_MemoryFree(Gizmos);
}

void HF_GizmosBeginLines(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View, HF_Matrix4 Model)
{
	HF_ShaderBind(Shader);

	HF_ShaderSet1Matrix4(Shader, "Projection", Projection);
	HF_ShaderSet1Matrix4(Shader, "View", View);
	HF_ShaderSet1Matrix4(Shader, "Model", Model);
}

void HF_GizmosDrawLine(struct HF_Gizmos *Gizmos, HF_Vector3 From, HF_Vector3 To, int unsigned Color)
{
	static struct HF_BufferLineVertex VertexBuffer[2];
	static int unsigned IndexBuffer[2];

	glBindVertexArray(Gizmos->LineVao);

	VertexBuffer[0].Position[0] = From[0];
	VertexBuffer[0].Position[1] = From[1];
	VertexBuffer[0].Position[2] = From[2];
	VertexBuffer[0].Color = Color;

	VertexBuffer[1].Position[0] = To[0];
	VertexBuffer[1].Position[1] = To[1];
	VertexBuffer[1].Position[2] = To[2];
	VertexBuffer[1].Color = Color;

	IndexBuffer[0] = 0;
	IndexBuffer[1] = 1;

	glBindBuffer(GL_ARRAY_BUFFER, Gizmos->LineVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct HF_BufferLineVertex) * 2, VertexBuffer, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Gizmos->LineEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int unsigned) * 2, IndexBuffer, GL_STREAM_DRAW);

	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void HF_GizmosEndLines(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader)
{
	HF_ShaderUnbind(Shader);
}

void HF_GizmosBeginQuads(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View, HF_Matrix4 Model)
{
	HF_ShaderBind(Shader);

	HF_ShaderSet1Matrix4(Shader, "Projection", Projection);
	HF_ShaderSet1Matrix4(Shader, "View", View);
	HF_ShaderSet1Matrix4(Shader, "Model", Model);
}

void HF_GizmosDrawQuad(struct HF_Gizmos *Gizmos, HF_Vector3 Position, HF_Vector2 Size, int unsigned Color)
{
	static struct HF_BufferQuadVertex VertexBuffer[4];
	static int unsigned IndexBuffer[6];

	glBindVertexArray(Gizmos->QuadVao);

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
	VertexBuffer[2].Position[1] = Position[1] + Size[1];
	VertexBuffer[2].Position[2] = Position[2];
	VertexBuffer[2].TextureCoords[0] = 1.0F;
	VertexBuffer[2].TextureCoords[1] = 1.0F;
	VertexBuffer[2].Color = Color;

	VertexBuffer[3].Position[0] = Position[0];
	VertexBuffer[3].Position[1] = Position[1] + Size[1];
	VertexBuffer[3].Position[2] = Position[2];
	VertexBuffer[3].TextureCoords[0] = 0.0F;
	VertexBuffer[3].TextureCoords[1] = 1.0F;
	VertexBuffer[3].Color = Color;

	IndexBuffer[0] = 0;
	IndexBuffer[1] = 1;
	IndexBuffer[2] = 2;

	IndexBuffer[3] = 2;
	IndexBuffer[4] = 3;
	IndexBuffer[5] = 0;

	glBindBuffer(GL_ARRAY_BUFFER, Gizmos->QuadVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct HF_BufferQuadVertex) * 4, VertexBuffer, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Gizmos->QuadEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int unsigned) * 6, IndexBuffer, GL_STREAM_DRAW);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void HF_GizmosEndQuads(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader)
{
	HF_ShaderUnbind(Shader);
}

void HF_GizmosBeginLineBatch(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View)
{
	HF_ShaderBind(Shader);

	HF_ShaderSet1Matrix4(Shader, "Projection", Projection);
	HF_ShaderSet1Matrix4(Shader, "View", View);
}

void HF_GizmosDrawBatchLine(struct HF_Gizmos *Gizmos, HF_Vector3 From, HF_Vector3 To, int unsigned Color)
{
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 0].Position[0] = From[0];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 0].Position[1] = From[1];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 0].Position[2] = From[2];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 0].Color = Color;

	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 1].Position[0] = To[0];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 1].Position[1] = To[1];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 1].Position[2] = To[2];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 1].Color = Color;

	Gizmos->IndexLineBatchBuffer[Gizmos->LineBatchIndexOffset + 0] = Gizmos->LineBatchVertexOffset + 0;
	Gizmos->IndexLineBatchBuffer[Gizmos->LineBatchIndexOffset + 1] = Gizmos->LineBatchVertexOffset + 1;

	Gizmos->LineBatchVertexOffset += 2;
	Gizmos->LineBatchIndexOffset += 2;
}

void HF_GizmosDrawBatchLineRect(struct HF_Gizmos *Gizmos, HF_Vector3 Position, HF_Vector2 Size, int unsigned Color)
{
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 0].Position[0] = Position[0];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 0].Position[1] = Position[1];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 0].Position[2] = Position[2];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 0].Color = Color;

	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 1].Position[0] = Position[0] + Size[0];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 1].Position[1] = Position[1];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 1].Position[2] = Position[2];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 1].Color = Color;

	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 2].Position[0] = Position[0] + Size[0];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 2].Position[1] = Position[1] + Size[1];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 2].Position[2] = Position[2];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 2].Color = Color;

	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 3].Position[0] = Position[0];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 3].Position[1] = Position[1] + Size[1];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 3].Position[2] = Position[2];
	Gizmos->VertexLineBatchBuffer[Gizmos->LineBatchVertexOffset + 3].Color = Color;

	Gizmos->IndexLineBatchBuffer[Gizmos->LineBatchIndexOffset + 0] = Gizmos->LineBatchVertexOffset + 0;
	Gizmos->IndexLineBatchBuffer[Gizmos->LineBatchIndexOffset + 1] = Gizmos->LineBatchVertexOffset + 1;

	Gizmos->IndexLineBatchBuffer[Gizmos->LineBatchIndexOffset + 2] = Gizmos->LineBatchVertexOffset + 1;
	Gizmos->IndexLineBatchBuffer[Gizmos->LineBatchIndexOffset + 3] = Gizmos->LineBatchVertexOffset + 2;

	Gizmos->IndexLineBatchBuffer[Gizmos->LineBatchIndexOffset + 4] = Gizmos->LineBatchVertexOffset + 2;
	Gizmos->IndexLineBatchBuffer[Gizmos->LineBatchIndexOffset + 5] = Gizmos->LineBatchVertexOffset + 3;

	Gizmos->IndexLineBatchBuffer[Gizmos->LineBatchIndexOffset + 6] = Gizmos->LineBatchVertexOffset + 3;
	Gizmos->IndexLineBatchBuffer[Gizmos->LineBatchIndexOffset + 7] = Gizmos->LineBatchVertexOffset + 0;

	Gizmos->LineBatchVertexOffset += 4;
	Gizmos->LineBatchIndexOffset += 8;
}

void HF_GizmosDrawLineBatch(struct HF_Gizmos *Gizmos)
{
	glBindVertexArray(Gizmos->LineBatchVao);

	glBindBuffer(GL_ARRAY_BUFFER, Gizmos->LineBatchVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct HF_BufferLineVertex) * Gizmos->LineBatchVertexOffset, Gizmos->VertexLineBatchBuffer, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Gizmos->LineBatchEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int unsigned) * Gizmos->LineBatchIndexOffset, Gizmos->IndexLineBatchBuffer, GL_STREAM_DRAW);

	glDrawElements(GL_LINES, Gizmos->LineBatchIndexOffset, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	Gizmos->LineBatchVertexOffset = 0;
	Gizmos->LineBatchIndexOffset = 0;
}

void HF_GizmosEndLineBatch(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader)
{
	HF_ShaderUnbind(Shader);
}

void HF_GizmosBeginQuadBatch(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View)
{
	HF_ShaderBind(Shader);

	HF_ShaderSet1Matrix4(Shader, "Projection", Projection);
	HF_ShaderSet1Matrix4(Shader, "View", View);
}

void HF_GizmosDrawBatchQuad(struct HF_Gizmos *Gizmos, HF_Vector3 Position, HF_Vector2 Size, int unsigned Color)
{
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 0].Position[0] = Position[0];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 0].Position[1] = Position[1];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 0].Position[2] = Position[2];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 0].TextureCoords[0] = 0.0F;
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 0].TextureCoords[1] = 0.0F;
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 0].Color = Color;

	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 1].Position[0] = Position[0] + Size[0];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 1].Position[1] = Position[1];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 1].Position[2] = Position[2];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 1].TextureCoords[0] = 1.0F;
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 1].TextureCoords[1] = 0.0F;
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 1].Color = Color;

	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 2].Position[0] = Position[0] + Size[0];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 2].Position[1] = Position[1] + Size[1];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 2].Position[2] = Position[2];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 2].TextureCoords[0] = 1.0F;
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 2].TextureCoords[1] = 1.0F;
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 2].Color = Color;

	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 3].Position[0] = Position[0];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 3].Position[1] = Position[1] + Size[1];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 3].Position[2] = Position[2];
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 3].TextureCoords[0] = 0.0F;
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 3].TextureCoords[1] = 1.0F;
	Gizmos->VertexQuadBatchBuffer[Gizmos->QuadBatchVertexOffset + 3].Color = Color;

	Gizmos->IndexQuadBatchBuffer[Gizmos->QuadBatchIndexOffset + 0] = Gizmos->QuadBatchVertexOffset + 0;
	Gizmos->IndexQuadBatchBuffer[Gizmos->QuadBatchIndexOffset + 1] = Gizmos->QuadBatchVertexOffset + 1;
	Gizmos->IndexQuadBatchBuffer[Gizmos->QuadBatchIndexOffset + 2] = Gizmos->QuadBatchVertexOffset + 2;

	Gizmos->IndexQuadBatchBuffer[Gizmos->QuadBatchIndexOffset + 3] = Gizmos->QuadBatchVertexOffset + 2;
	Gizmos->IndexQuadBatchBuffer[Gizmos->QuadBatchIndexOffset + 4] = Gizmos->QuadBatchVertexOffset + 3;
	Gizmos->IndexQuadBatchBuffer[Gizmos->QuadBatchIndexOffset + 5] = Gizmos->QuadBatchVertexOffset + 0;

	Gizmos->QuadBatchVertexOffset += 4;
	Gizmos->QuadBatchIndexOffset += 6;
}

void HF_GizmosDrawQuadBatch(struct HF_Gizmos *Gizmos)
{
	glBindVertexArray(Gizmos->QuadBatchVao);

	glBindBuffer(GL_ARRAY_BUFFER, Gizmos->QuadBatchVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct HF_BufferQuadVertex) * Gizmos->QuadBatchVertexOffset, Gizmos->VertexQuadBatchBuffer, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Gizmos->QuadBatchEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int unsigned) * Gizmos->QuadBatchIndexOffset, Gizmos->IndexQuadBatchBuffer, GL_STREAM_DRAW);

	glDrawElements(GL_TRIANGLES, Gizmos->QuadBatchIndexOffset, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	Gizmos->QuadBatchVertexOffset = 0;
	Gizmos->QuadBatchIndexOffset = 0;
}

void HF_GizmosEndQuadBatch(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader)
{
	HF_ShaderUnbind(Shader);
}