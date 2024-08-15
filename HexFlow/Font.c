#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <HexFlow/Font.h>
#include <HexFlow/Shader.h>
#include <HexFlow/Memory.h>

#include <HexFlow/FreeType/freetype.h>

#include <HexFlow/GLAD/glad.h>

#ifdef HF_DEBUG

	#define FT_CHECK(EXPRESSION) \
		{ \
			FT_Error Error = (EXPRESSION); \
			if (Error) \
			{ \
				const char* ErrorStr = FT_Error_String(Error); \
				if (ErrorStr) \
				{ \
					printf(ErrorStr); \
				} \
			} \
		}

#else

	#define FT_CHECK(EXPRESSION) (EXPRESSION)

#endif // HF_DEBUG

#define HF_BUFFER_GLYPH_SIZE 0x100
#define HF_BUFFER_CURVE_SIZE 0x1000

#define HF_VERTEX_BUFFER_SIZE 0x10000
#define HF_INDEX_BUFFER_SIZE 0x20000

#define HF_GLYPH_SIZE 0x400

struct HF_Glyph
{
	int unsigned Index, BufferIndex;
	int unsigned CurveCount;

	int Width, Height;
	int BearingX, BearingY;
	int Advance;
};

struct HF_BufferVertex
{
	HF_Vector3 Position;
	HF_Vector2 TextureCoords;

	int unsigned BufferIndex, Color;
};

struct HF_BufferGlyph
{
	unsigned int Start, Count;
};

struct HF_BufferCurve
{
	HF_Vector2 P0, P1, P2;
};

struct HF_Font
{
	int unsigned Vao, Vbo, Ebo;
	int unsigned GlyphTexture, CurveTexture;
	int unsigned GlyphBuffer, CurveBuffer;

	float WorldSize;
	float EmSize;
	float Height;
	float Dilation;

	struct HF_BufferGlyph BufferGlyphs[HF_BUFFER_GLYPH_SIZE];
	struct HF_BufferCurve BufferCurves[HF_BUFFER_CURVE_SIZE];

	int unsigned BufferGlyphOffset;
	int unsigned BufferCurveOffset;

	struct HF_Glyph Glyphs[HF_GLYPH_SIZE];

	int LoadFlags;

	FT_Face Face;
	FT_Kerning_Mode KerningMode;

	struct HF_BufferVertex VertexBuffer[HF_VERTEX_BUFFER_SIZE];
	int unsigned IndexBuffer[HF_INDEX_BUFFER_SIZE];

	int unsigned VertexBufferOffset;
	int unsigned IndexBufferOffset;
};

static FT_Library m_Library = 0;

static void HF_FontBuildGlyph(struct HF_Font *Font, int unsigned CharCode, int unsigned GlyphIndex);
static void HF_FontConvertContour(struct HF_Font *Font, short FirstIndex, short LastIndex, float EmSize);
static void HF_FontUploadBuffers(struct HF_Font *Font);

void HF_FontInitFreeType(void)
{
	FT_CHECK(FT_Init_FreeType(&m_Library));
}

void HF_FontDeinitFreeType(void)
{
	FT_CHECK(FT_Done_FreeType(m_Library));
}

struct HF_Font* HF_FontAlloc(char unsigned const *FileBase, long long unsigned FileSize, float WorldSize)
{
	struct HF_Font* Font = (struct HF_Font*)HF_MemoryAlloc(sizeof(struct HF_Font), 0);

	FT_CHECK(FT_New_Memory_Face(m_Library, FileBase, (int unsigned)FileSize, 0, &Font->Face));

	Font->WorldSize = WorldSize;
	Font->Dilation = 0.0F;
	Font->LoadFlags = FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
	Font->KerningMode = FT_KERNING_UNSCALED;
	Font->Height = Font->Face->height;
	Font->EmSize = Font->Face->units_per_EM;

	glGenVertexArrays(1, &Font->Vao);

	glGenBuffers(1, &Font->Vbo);
	glGenBuffers(1, &Font->Ebo);

	glGenTextures(1, &Font->GlyphTexture);
	glGenTextures(1, &Font->CurveTexture);

	glGenBuffers(1, &Font->GlyphBuffer);
	glGenBuffers(1, &Font->CurveBuffer);

	glBindVertexArray(Font->Vao);
	glBindBuffer(GL_ARRAY_BUFFER, Font->Vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Font->Ebo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(struct HF_BufferVertex), (void*)(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(struct HF_BufferVertex), (void*)(sizeof(HF_Vector3)));
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(struct HF_BufferVertex), (void*)(sizeof(HF_Vector3) + sizeof(HF_Vector2)));
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(struct HF_BufferVertex), (void*)(sizeof(HF_Vector3) + sizeof(HF_Vector2) + sizeof(int unsigned)));
	glBindVertexArray(0);

	FT_CHECK(FT_Load_Glyph(Font->Face, 0, Font->LoadFlags));

	HF_FontBuildGlyph(Font, 0, 0);

	for (int unsigned CharCode = 32; CharCode < 128; CharCode++)
	{
		int unsigned GlyphIndex = FT_Get_Char_Index(Font->Face, CharCode);

		if (GlyphIndex)
		{
			FT_CHECK(FT_Load_Glyph(Font->Face, GlyphIndex, Font->LoadFlags));

			HF_FontBuildGlyph(Font, CharCode, GlyphIndex);
		}
	}

	HF_FontUploadBuffers(Font);

	glBindTexture(GL_TEXTURE_BUFFER, Font->GlyphTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RG32UI, Font->GlyphBuffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	glBindTexture(GL_TEXTURE_BUFFER, Font->CurveTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RG32F, Font->CurveBuffer);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	return Font;
}

void HF_FontFree(struct HF_Font *Font)
{
	glDeleteVertexArrays(1, &Font->Vao);

	glDeleteBuffers(1, &Font->Vbo);
	glDeleteBuffers(1, &Font->Ebo);

	glDeleteTextures(1, &Font->GlyphTexture);
	glDeleteTextures(1, &Font->CurveTexture);

	glDeleteBuffers(1, &Font->GlyphBuffer);
	glDeleteBuffers(1, &Font->CurveBuffer);

	FT_Done_Face(Font->Face);

	HF_MemoryFree(Font);
}

void HF_FontBeginDraw(struct HF_Font *Font, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View, HF_Matrix4 Model)
{
	HF_ShaderBind(Shader);

	HF_ShaderSet1Matrix4(Shader, "Projection", Projection);
	HF_ShaderSet1Matrix4(Shader, "View", View);
	HF_ShaderSet1Matrix4(Shader, "Model", Model);

	HF_ShaderSet1Int32(Shader, "Glyphs", 0);
	HF_ShaderSet1Int32(Shader, "Curves", 1);
	HF_ShaderSet1Real32(Shader, "AntiAliasingWindowSize", 1.0F);
	HF_ShaderSet1Int32(Shader, "EnableSuperSamplingAntiAliasing", 1);
	HF_ShaderSet1Int32(Shader, "EnableControlPointsVisualization", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, Font->GlyphTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_BUFFER, Font->CurveTexture);

	glActiveTexture(GL_TEXTURE0);
}

void HF_FontDrawText(struct HF_Font *Font, HF_Vector3 Position, HF_Vector2 Size, char const *Text)
{
	float X = Position[0], Y = Position[1];

	Y -= Font->Height / ((float)Font->EmSize * Font->WorldSize);

	glBindVertexArray(Font->Vao);

	int unsigned PrevGlyphIndex = 0;

	for (char const *CharCode = Text; *CharCode != '\0'; CharCode++)
	{
		switch (*CharCode)
		{
			case '\r':
			{
				break;
			}
			case '\n':
			{
				X = Position[0];
				Y -= Font->Height / ((float)Font->EmSize * Font->WorldSize);

				break;
			}
			default:
			{
				struct HF_Glyph *Glyph = &Font->Glyphs[*CharCode];

				if ((PrevGlyphIndex != 0) && (Glyph->Index != 0))
				{
					FT_Vector Kerning = { 0 };

					FT_CHECK(FT_Get_Kerning(Font->Face, PrevGlyphIndex, Glyph->Index, Font->KerningMode, &Kerning));

					X += (float)Kerning.x / (Font->EmSize * Font->WorldSize);
				}

				if (Glyph->CurveCount)
				{
					float D = Font->EmSize * Font->Dilation;

					float U0 = ((float)Glyph->BearingX - D) / Font->EmSize;
					float V0 = ((float)Glyph->BearingY - (float)Glyph->Height - D) / Font->EmSize;
					float U1 = ((float)Glyph->BearingX + (float)Glyph->Width + D) / Font->EmSize;
					float V1 = ((float)Glyph->BearingY + D) / Font->EmSize;

					float X0 = X + U0 * Font->WorldSize;
					float Y0 = Y + V0 * Font->WorldSize;
					float X1 = X + U1 * Font->WorldSize;
					float Y1 = Y + V1 * Font->WorldSize;

					if (fabsf(X1) > Size[0]) Size[0] = fabsf(X1);
					if (fabsf(Y0) > Size[1]) Size[1] = fabsf(Y0);

					Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[0] = X0;
					Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[1] = Y0;
					Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[2] = Position[2];
					Font->VertexBuffer[Font->VertexBufferOffset + 0].TextureCoords[0] = U0;
					Font->VertexBuffer[Font->VertexBufferOffset + 0].TextureCoords[1] = V0;
					Font->VertexBuffer[Font->VertexBufferOffset + 0].BufferIndex = Glyph->BufferIndex;
					Font->VertexBuffer[Font->VertexBufferOffset + 0].Color = 0xFFFFFFFF;

					Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[0] = X1;
					Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[1] = Y0;
					Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[2] = Position[2];
					Font->VertexBuffer[Font->VertexBufferOffset + 1].TextureCoords[0] = U1;
					Font->VertexBuffer[Font->VertexBufferOffset + 1].TextureCoords[1] = V0;
					Font->VertexBuffer[Font->VertexBufferOffset + 1].BufferIndex = Glyph->BufferIndex;
					Font->VertexBuffer[Font->VertexBufferOffset + 1].Color = 0xFFFFFFFF;

					Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[0] = X1;
					Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[1] = Y1;
					Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[2] = Position[2];
					Font->VertexBuffer[Font->VertexBufferOffset + 2].TextureCoords[0] = U1;
					Font->VertexBuffer[Font->VertexBufferOffset + 2].TextureCoords[1] = V1;
					Font->VertexBuffer[Font->VertexBufferOffset + 2].BufferIndex = Glyph->BufferIndex;
					Font->VertexBuffer[Font->VertexBufferOffset + 2].Color = 0xFFFFFFFF;

					Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[0] = X0;
					Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[1] = Y1;
					Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[2] = Position[2];
					Font->VertexBuffer[Font->VertexBufferOffset + 3].TextureCoords[0] = U0;
					Font->VertexBuffer[Font->VertexBufferOffset + 3].TextureCoords[1] = V1;
					Font->VertexBuffer[Font->VertexBufferOffset + 3].BufferIndex = Glyph->BufferIndex;
					Font->VertexBuffer[Font->VertexBufferOffset + 3].Color = 0xFFFFFFFF;

					Font->IndexBuffer[Font->IndexBufferOffset + 0] = Font->VertexBufferOffset + 0;
					Font->IndexBuffer[Font->IndexBufferOffset + 1] = Font->VertexBufferOffset + 1;
					Font->IndexBuffer[Font->IndexBufferOffset + 2] = Font->VertexBufferOffset + 2;

					Font->IndexBuffer[Font->IndexBufferOffset + 3] = Font->VertexBufferOffset + 2;
					Font->IndexBuffer[Font->IndexBufferOffset + 4] = Font->VertexBufferOffset + 3;
					Font->IndexBuffer[Font->IndexBufferOffset + 5] = Font->VertexBufferOffset + 0;

					Font->VertexBufferOffset += 4;
					Font->IndexBufferOffset += 6;
				}

				X += (float)Glyph->Advance / (Font->EmSize * Font->WorldSize);

				PrevGlyphIndex = Glyph->Index;

				break;
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, Font->Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct HF_BufferVertex) * Font->VertexBufferOffset, Font->VertexBuffer, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Font->Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int unsigned) * Font->IndexBufferOffset, Font->IndexBuffer, GL_STREAM_DRAW);

	glDrawElements(GL_TRIANGLES, Font->IndexBufferOffset, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	Font->VertexBufferOffset = 0;
	Font->IndexBufferOffset = 0;

	Size[0] -= Position[0];
	Size[1] -= Position[1];
}

void HF_FontDrawTextClipped(struct HF_Font *Font, HF_Vector3 Position, HF_Vector2 Size, char const *Text)
{
	float X = Position[0], Y = Position[1];

	Y -= Font->Height / ((float)Font->EmSize * Font->WorldSize);

	glBindVertexArray(Font->Vao);

	int unsigned PrevGlyphIndex = 0;

	char unsigned FastForwardTillEndOfLine = 0;
	char unsigned FastForwardTillEndOfFile = 0;

	for (char const *CharCode = Text; *CharCode != '\0'; CharCode++)
	{
		switch (*CharCode)
		{
			case '\r':
			{
				break;
			}
			case '\n':
			{
				X = Position[0];
				Y -= Font->Height / ((float)Font->EmSize * Font->WorldSize);

				FastForwardTillEndOfLine = 0;

				if (FastForwardTillEndOfFile == 1)
				{
					goto HF_FontDrawTextClipped_Draw;
				}

				break;
			}
			default:
			{
				if (FastForwardTillEndOfLine == 0)
				{
					struct HF_Glyph *Glyph = &Font->Glyphs[*CharCode];

					if ((PrevGlyphIndex != 0) && (Glyph->Index != 0))
					{
						FT_Vector Kerning = { 0 };

						FT_CHECK(FT_Get_Kerning(Font->Face, PrevGlyphIndex, Glyph->Index, Font->KerningMode, &Kerning));

						X += (float)Kerning.x / (Font->EmSize * Font->WorldSize);
					}

					if (Glyph->CurveCount)
					{
						float D = Font->EmSize * Font->Dilation;

						float U0 = ((float)Glyph->BearingX - D) / Font->EmSize;
						float V0 = ((float)Glyph->BearingY - (float)Glyph->Height - D) / Font->EmSize;
						float U1 = ((float)Glyph->BearingX + (float)Glyph->Width + D) / Font->EmSize;
						float V1 = ((float)Glyph->BearingY + D) / Font->EmSize;

						float X0 = X + U0 * Font->WorldSize;
						float Y0 = Y + V0 * Font->WorldSize;
						float X1 = X + U1 * Font->WorldSize;
						float Y1 = Y + V1 * Font->WorldSize;

						if (X1 > (Position[0] + Size[0]))
						{
							X1 = (Position[0] + Size[0]);

							FastForwardTillEndOfLine = 1;
						}

						if (Y0 < (Position[1] - Size[1]))
						{
							Y0 = (Position[1] - Size[1]);

							FastForwardTillEndOfFile = 1;
						}

						Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[0] = X0;
						Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[1] = Y0;
						Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[2] = Position[2];
						Font->VertexBuffer[Font->VertexBufferOffset + 0].TextureCoords[0] = U0;
						Font->VertexBuffer[Font->VertexBufferOffset + 0].TextureCoords[1] = V0;
						Font->VertexBuffer[Font->VertexBufferOffset + 0].BufferIndex = Glyph->BufferIndex;
						Font->VertexBuffer[Font->VertexBufferOffset + 0].Color = 0xFFFFFFFF;

						Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[0] = X1;
						Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[1] = Y0;
						Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[2] = Position[2];
						Font->VertexBuffer[Font->VertexBufferOffset + 1].TextureCoords[0] = U1;
						Font->VertexBuffer[Font->VertexBufferOffset + 1].TextureCoords[1] = V0;
						Font->VertexBuffer[Font->VertexBufferOffset + 1].BufferIndex = Glyph->BufferIndex;
						Font->VertexBuffer[Font->VertexBufferOffset + 1].Color = 0xFFFFFFFF;

						Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[0] = X1;
						Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[1] = Y1;
						Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[2] = Position[2];
						Font->VertexBuffer[Font->VertexBufferOffset + 2].TextureCoords[0] = U1;
						Font->VertexBuffer[Font->VertexBufferOffset + 2].TextureCoords[1] = V1;
						Font->VertexBuffer[Font->VertexBufferOffset + 2].BufferIndex = Glyph->BufferIndex;
						Font->VertexBuffer[Font->VertexBufferOffset + 2].Color = 0xFFFFFFFF;

						Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[0] = X0;
						Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[1] = Y1;
						Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[2] = Position[2];
						Font->VertexBuffer[Font->VertexBufferOffset + 3].TextureCoords[0] = U0;
						Font->VertexBuffer[Font->VertexBufferOffset + 3].TextureCoords[1] = V1;
						Font->VertexBuffer[Font->VertexBufferOffset + 3].BufferIndex = Glyph->BufferIndex;
						Font->VertexBuffer[Font->VertexBufferOffset + 3].Color = 0xFFFFFFFF;

						Font->IndexBuffer[Font->IndexBufferOffset + 0] = Font->VertexBufferOffset + 0;
						Font->IndexBuffer[Font->IndexBufferOffset + 1] = Font->VertexBufferOffset + 1;
						Font->IndexBuffer[Font->IndexBufferOffset + 2] = Font->VertexBufferOffset + 2;

						Font->IndexBuffer[Font->IndexBufferOffset + 3] = Font->VertexBufferOffset + 2;
						Font->IndexBuffer[Font->IndexBufferOffset + 4] = Font->VertexBufferOffset + 3;
						Font->IndexBuffer[Font->IndexBufferOffset + 5] = Font->VertexBufferOffset + 0;

						Font->VertexBufferOffset += 4;
						Font->IndexBufferOffset += 6;
					}

					X += (float)Glyph->Advance / (Font->EmSize * Font->WorldSize);

					PrevGlyphIndex = Glyph->Index;
				}

				break;
			}
		}
	}

HF_FontDrawTextClipped_Draw:

	glBindBuffer(GL_ARRAY_BUFFER, Font->Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct HF_BufferVertex) * Font->VertexBufferOffset, Font->VertexBuffer, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Font->Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int unsigned) * Font->IndexBufferOffset, Font->IndexBuffer, GL_STREAM_DRAW);

	glDrawElements(GL_TRIANGLES, Font->IndexBufferOffset, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	Font->VertexBufferOffset = 0;
	Font->IndexBufferOffset = 0;
}

void HF_FontDrawHex(struct HF_Font *Font, HF_Vector3 Position, HF_Vector2 Size, char unsigned const *Buffer, long long unsigned BufferSize, int unsigned NumOfColumns, float Spacing)
{
	float X = Position[0], Y = Position[1];

	Y -= Font->Height / ((float)Font->EmSize * Font->WorldSize);

	glBindVertexArray(Font->Vao);

	int unsigned PrevGlyphIndex = 0;

	for (long long unsigned BufferIndex = 0; BufferIndex < BufferSize; BufferIndex++)
	{
		if ((BufferIndex != 0) && (BufferIndex % NumOfColumns) == 0)
		{
			X = Position[0];
			Y -= Font->Height / ((float)Font->EmSize * Font->WorldSize);
		}

		char HexRepresentation[3] = { 0 };

		snprintf(HexRepresentation, sizeof(HexRepresentation), "%02X", Buffer[BufferIndex]);

		for (char unsigned CharIndex = 0; CharIndex < 2; CharIndex++)
		{
			char unsigned CharCode = HexRepresentation[CharIndex];

			struct HF_Glyph *Glyph = &Font->Glyphs[CharCode];

			if ((PrevGlyphIndex != 0) && (Glyph->Index != 0))
			{
				FT_Vector Kerning = { 0 };

				FT_CHECK(FT_Get_Kerning(Font->Face, PrevGlyphIndex, Glyph->Index, Font->KerningMode, &Kerning));

				X += (float)Kerning.x / (Font->EmSize * Font->WorldSize);
			}

			if (Glyph->CurveCount)
			{
				float D = Font->EmSize * Font->Dilation;

				float U0 = ((float)Glyph->BearingX - D) / Font->EmSize;
				float V0 = ((float)Glyph->BearingY - (float)Glyph->Height - D) / Font->EmSize;
				float U1 = ((float)Glyph->BearingX + (float)Glyph->Width + D) / Font->EmSize;
				float V1 = ((float)Glyph->BearingY + D) / Font->EmSize;

				float X0 = X + U0 * Font->WorldSize;
				float Y0 = Y + V0 * Font->WorldSize;
				float X1 = X + U1 * Font->WorldSize;
				float Y1 = Y + V1 * Font->WorldSize;

				if (fabsf(X1) > Size[0]) Size[0] = fabsf(X1);
				if (fabsf(Y0) > Size[1]) Size[1] = fabsf(Y0);

				Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[0] = X0;
				Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[1] = Y0;
				Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[2] = Position[2];
				Font->VertexBuffer[Font->VertexBufferOffset + 0].TextureCoords[0] = U0;
				Font->VertexBuffer[Font->VertexBufferOffset + 0].TextureCoords[1] = V0;
				Font->VertexBuffer[Font->VertexBufferOffset + 0].BufferIndex = Glyph->BufferIndex;
				Font->VertexBuffer[Font->VertexBufferOffset + 0].Color = 0xFFFFFFFF;

				Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[0] = X1;
				Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[1] = Y0;
				Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[2] = Position[2];
				Font->VertexBuffer[Font->VertexBufferOffset + 1].TextureCoords[0] = U1;
				Font->VertexBuffer[Font->VertexBufferOffset + 1].TextureCoords[1] = V0;
				Font->VertexBuffer[Font->VertexBufferOffset + 1].BufferIndex = Glyph->BufferIndex;
				Font->VertexBuffer[Font->VertexBufferOffset + 1].Color = 0xFFFFFFFF;

				Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[0] = X1;
				Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[1] = Y1;
				Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[2] = Position[2];
				Font->VertexBuffer[Font->VertexBufferOffset + 2].TextureCoords[0] = U1;
				Font->VertexBuffer[Font->VertexBufferOffset + 2].TextureCoords[1] = V1;
				Font->VertexBuffer[Font->VertexBufferOffset + 2].BufferIndex = Glyph->BufferIndex;
				Font->VertexBuffer[Font->VertexBufferOffset + 2].Color = 0xFFFFFFFF;

				Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[0] = X0;
				Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[1] = Y1;
				Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[2] = Position[2];
				Font->VertexBuffer[Font->VertexBufferOffset + 3].TextureCoords[0] = U0;
				Font->VertexBuffer[Font->VertexBufferOffset + 3].TextureCoords[1] = V1;
				Font->VertexBuffer[Font->VertexBufferOffset + 3].BufferIndex = Glyph->BufferIndex;
				Font->VertexBuffer[Font->VertexBufferOffset + 3].Color = 0xFFFFFFFF;

				Font->IndexBuffer[Font->IndexBufferOffset + 0] = Font->VertexBufferOffset + 0;
				Font->IndexBuffer[Font->IndexBufferOffset + 1] = Font->VertexBufferOffset + 1;
				Font->IndexBuffer[Font->IndexBufferOffset + 2] = Font->VertexBufferOffset + 2;

				Font->IndexBuffer[Font->IndexBufferOffset + 3] = Font->VertexBufferOffset + 2;
				Font->IndexBuffer[Font->IndexBufferOffset + 4] = Font->VertexBufferOffset + 3;
				Font->IndexBuffer[Font->IndexBufferOffset + 5] = Font->VertexBufferOffset + 0;

				Font->VertexBufferOffset += 4;
				Font->IndexBufferOffset += 6;
			}

			X += (float)Glyph->Advance / (Font->EmSize * Font->WorldSize);

			PrevGlyphIndex = Glyph->Index;
		}

		X += Spacing * Font->WorldSize;
	}

	glBindBuffer(GL_ARRAY_BUFFER, Font->Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct HF_BufferVertex) * Font->VertexBufferOffset, Font->VertexBuffer, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Font->Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int unsigned) * Font->IndexBufferOffset, Font->IndexBuffer, GL_STREAM_DRAW);

	glDrawElements(GL_TRIANGLES, Font->IndexBufferOffset, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	Font->VertexBufferOffset = 0;
	Font->IndexBufferOffset = 0;

	Size[0] -= Position[0];
	Size[1] -= Position[1];
}

void HF_FontDrawHexClipped(struct HF_Font *Font, HF_Vector3 Position, HF_Vector2 Size, char unsigned const *Buffer, long long unsigned BufferSize, int unsigned NumOfColumns, float Spacing)
{
	float X = Position[0], Y = Position[1];

	Y -= Font->Height / ((float)Font->EmSize * Font->WorldSize);

	glBindVertexArray(Font->Vao);

	int unsigned PrevGlyphIndex = 0;

	char unsigned FastForwardTillEndOfLine = 0;
	char unsigned FastForwardTillEndOfFile = 0;

	for (long long unsigned BufferIndex = 0; BufferIndex < BufferSize; BufferIndex++)
	{
		if ((BufferIndex != 0) && (BufferIndex % NumOfColumns) == 0)
		{
			X = Position[0];
			Y -= Font->Height / ((float)Font->EmSize * Font->WorldSize);

			FastForwardTillEndOfLine = 0;

			if (FastForwardTillEndOfFile == 1)
			{
				goto HF_FontDrawHexClipped_Draw;
			}
		}

		char HexRepresentation[3] = { 0 };

		snprintf(HexRepresentation, sizeof(HexRepresentation), "%02X", Buffer[BufferIndex]);

		for (char unsigned CharIndex = 0; CharIndex < 2; CharIndex++)
		{
			if (FastForwardTillEndOfLine == 0)
			{
				char unsigned CharCode = HexRepresentation[CharIndex];

				struct HF_Glyph *Glyph = &Font->Glyphs[CharCode];

				if ((PrevGlyphIndex != 0) && (Glyph->Index != 0))
				{
					FT_Vector Kerning = { 0 };

					FT_CHECK(FT_Get_Kerning(Font->Face, PrevGlyphIndex, Glyph->Index, Font->KerningMode, &Kerning));

					X += (float)Kerning.x / (Font->EmSize * Font->WorldSize);
				}

				if (Glyph->CurveCount)
				{
					float D = Font->EmSize * Font->Dilation;

					float U0 = ((float)Glyph->BearingX - D) / Font->EmSize;
					float V0 = ((float)Glyph->BearingY - (float)Glyph->Height - D) / Font->EmSize;
					float U1 = ((float)Glyph->BearingX + (float)Glyph->Width + D) / Font->EmSize;
					float V1 = ((float)Glyph->BearingY + D) / Font->EmSize;

					float X0 = X + U0 * Font->WorldSize;
					float Y0 = Y + V0 * Font->WorldSize;
					float X1 = X + U1 * Font->WorldSize;
					float Y1 = Y + V1 * Font->WorldSize;

					if (X1 > (Position[0] + Size[0]))
					{
						X1 = (Position[0] + Size[0]);

						FastForwardTillEndOfLine = 1;
					}

					if (Y0 < (Position[1] - Size[1]))
					{
						Y0 = (Position[1] - Size[1]);

						FastForwardTillEndOfFile = 1;
					}

					Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[0] = X0;
					Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[1] = Y0;
					Font->VertexBuffer[Font->VertexBufferOffset + 0].Position[2] = Position[2];
					Font->VertexBuffer[Font->VertexBufferOffset + 0].TextureCoords[0] = U0;
					Font->VertexBuffer[Font->VertexBufferOffset + 0].TextureCoords[1] = V0;
					Font->VertexBuffer[Font->VertexBufferOffset + 0].BufferIndex = Glyph->BufferIndex;
					Font->VertexBuffer[Font->VertexBufferOffset + 0].Color = 0xFFFFFFFF;

					Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[0] = X1;
					Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[1] = Y0;
					Font->VertexBuffer[Font->VertexBufferOffset + 1].Position[2] = Position[2];
					Font->VertexBuffer[Font->VertexBufferOffset + 1].TextureCoords[0] = U1;
					Font->VertexBuffer[Font->VertexBufferOffset + 1].TextureCoords[1] = V0;
					Font->VertexBuffer[Font->VertexBufferOffset + 1].BufferIndex = Glyph->BufferIndex;
					Font->VertexBuffer[Font->VertexBufferOffset + 1].Color = 0xFFFFFFFF;

					Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[0] = X1;
					Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[1] = Y1;
					Font->VertexBuffer[Font->VertexBufferOffset + 2].Position[2] = Position[2];
					Font->VertexBuffer[Font->VertexBufferOffset + 2].TextureCoords[0] = U1;
					Font->VertexBuffer[Font->VertexBufferOffset + 2].TextureCoords[1] = V1;
					Font->VertexBuffer[Font->VertexBufferOffset + 2].BufferIndex = Glyph->BufferIndex;
					Font->VertexBuffer[Font->VertexBufferOffset + 2].Color = 0xFFFFFFFF;

					Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[0] = X0;
					Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[1] = Y1;
					Font->VertexBuffer[Font->VertexBufferOffset + 3].Position[2] = Position[2];
					Font->VertexBuffer[Font->VertexBufferOffset + 3].TextureCoords[0] = U0;
					Font->VertexBuffer[Font->VertexBufferOffset + 3].TextureCoords[1] = V1;
					Font->VertexBuffer[Font->VertexBufferOffset + 3].BufferIndex = Glyph->BufferIndex;
					Font->VertexBuffer[Font->VertexBufferOffset + 3].Color = 0xFFFFFFFF;

					Font->IndexBuffer[Font->IndexBufferOffset + 0] = Font->VertexBufferOffset + 0;
					Font->IndexBuffer[Font->IndexBufferOffset + 1] = Font->VertexBufferOffset + 1;
					Font->IndexBuffer[Font->IndexBufferOffset + 2] = Font->VertexBufferOffset + 2;

					Font->IndexBuffer[Font->IndexBufferOffset + 3] = Font->VertexBufferOffset + 2;
					Font->IndexBuffer[Font->IndexBufferOffset + 4] = Font->VertexBufferOffset + 3;
					Font->IndexBuffer[Font->IndexBufferOffset + 5] = Font->VertexBufferOffset + 0;

					Font->VertexBufferOffset += 4;
					Font->IndexBufferOffset += 6;
				}

				X += (float)Glyph->Advance / (Font->EmSize * Font->WorldSize);

				PrevGlyphIndex = Glyph->Index;
			}
		}

		X += Spacing * Font->WorldSize;
	}

HF_FontDrawHexClipped_Draw:

	glBindBuffer(GL_ARRAY_BUFFER, Font->Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(struct HF_BufferVertex) * Font->VertexBufferOffset, Font->VertexBuffer, GL_STREAM_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Font->Ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int unsigned) * Font->IndexBufferOffset, Font->IndexBuffer, GL_STREAM_DRAW);

	glDrawElements(GL_TRIANGLES, Font->IndexBufferOffset, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	Font->VertexBufferOffset = 0;
	Font->IndexBufferOffset = 0;
}

void HF_FontEndDraw(struct HF_Font *Font, struct HF_Shader *Shader)
{
	HF_ShaderUnbind(Shader);
}

static void HF_FontBuildGlyph(struct HF_Font *Font, int unsigned CharCode, int unsigned GlyphIndex)
{
	Font->BufferGlyphs[Font->BufferGlyphOffset].Start = Font->BufferCurveOffset;

	short FirstIndex = 0;
	short LastIndex = 0;

	for (short ContourIndex = 0; ContourIndex < Font->Face->glyph->outline.n_contours; ContourIndex++)
	{
		LastIndex = Font->Face->glyph->outline.contours[ContourIndex];

		HF_FontConvertContour(Font, FirstIndex, LastIndex, Font->EmSize);

		FirstIndex = Font->Face->glyph->outline.contours[ContourIndex] + 1;
	}

	Font->BufferGlyphs[Font->BufferGlyphOffset].Count = Font->BufferCurveOffset - Font->BufferGlyphs[Font->BufferGlyphOffset].Start;

	Font->Glyphs[CharCode].Index = GlyphIndex;
	Font->Glyphs[CharCode].BufferIndex = Font->BufferGlyphOffset;
	Font->Glyphs[CharCode].CurveCount = Font->BufferGlyphs[Font->BufferGlyphOffset].Count;
	Font->Glyphs[CharCode].Width = Font->Face->glyph->metrics.width;
	Font->Glyphs[CharCode].Height = Font->Face->glyph->metrics.height;
	Font->Glyphs[CharCode].BearingX = Font->Face->glyph->metrics.horiBearingX;
	Font->Glyphs[CharCode].BearingY = Font->Face->glyph->metrics.horiBearingY;
	Font->Glyphs[CharCode].Advance = Font->Face->glyph->metrics.horiAdvance;

	Font->BufferGlyphOffset++;
}

static void HF_FontConvertContour(struct HF_Font *Font, short FirstIndex, short LastIndex, float EmSize)
{
	const FT_Outline *Outline = &Font->Face->glyph->outline;

	if (FirstIndex == LastIndex)
	{
		return;
	}

	short DirectionIndex = 1;

	if (Outline->flags & FT_OUTLINE_REVERSE_FILL)
	{
		short TmpIndex = LastIndex;

		LastIndex = FirstIndex;
		FirstIndex = TmpIndex;

		DirectionIndex = -1;
	}

	float FirstX, FirstY = 0.0F;

	char unsigned FirstOnCurve = (Outline->tags[FirstIndex] & FT_CURVE_TAG_ON);

	if (FirstOnCurve)
	{
		FirstX = Outline->points[FirstIndex].x / Font->EmSize;
		FirstY = Outline->points[FirstIndex].y / Font->EmSize;

		FirstIndex += DirectionIndex;
	}
	else
	{
		char unsigned LastOnCurve = (Outline->tags[LastIndex] & FT_CURVE_TAG_ON);

		if (LastOnCurve)
		{
			FirstX = Outline->points[LastIndex].x / Font->EmSize;
			FirstY = Outline->points[LastIndex].y / Font->EmSize;

			LastIndex -= DirectionIndex;
		}
		else
		{
			float XF = Outline->points[FirstIndex].x / Font->EmSize;
			float YF = Outline->points[FirstIndex].y / Font->EmSize;

			float XL = Outline->points[LastIndex].x / Font->EmSize;
			float YL = Outline->points[LastIndex].y / Font->EmSize;

			FirstX = 0.5F * (XF + XL);
			FirstY = 0.5F * (YF + YL);
		}
	}

	float StartX = FirstX, StartY = FirstY;
	float ControlX = FirstX, ControlY = FirstY;
	float PrevX = FirstX, PrevY = FirstY;

	char PrevTag = FT_CURVE_TAG_ON;

	for (short Index = FirstIndex; Index != LastIndex + DirectionIndex; Index += DirectionIndex)
	{
		float CurrX = Outline->points[Index].x / Font->EmSize;
		float CurrY = Outline->points[Index].y / Font->EmSize;

		char CurrTag = FT_CURVE_TAG(Outline->tags[Index]);

		if (CurrTag == FT_CURVE_TAG_CUBIC)
		{
			ControlX = PrevX;
			ControlY = PrevY;
		}
		else if (CurrTag == FT_CURVE_TAG_ON)
		{
			if (PrevTag == FT_CURVE_TAG_CUBIC)
			{
				float B0X = StartX;
				float B0Y = StartY;

				float B1X = ControlX;
				float B1Y = ControlY;

				float B2X = PrevX;
				float B2Y = PrevY;

				float B3X = CurrX;
				float B3Y = CurrY;

				float C0X = B0X + 0.75F * (B1X - B0X);
				float C0Y = B0Y + 0.75F * (B1Y - B0Y);

				float C1X = B3X + 0.75F * (B2X - B3X);
				float C1Y = B3Y + 0.75F * (B2Y - B3Y);

				float DX = 0.5F * (C0X + C1X);
				float DY = 0.5F * (C0Y + C1Y);

				{
					struct HF_BufferCurve Curve = { 0 };

					Curve.P0[0] = B0X;
					Curve.P0[1] = B0Y;

					Curve.P1[0] = C0X;
					Curve.P1[1] = C0Y;

					Curve.P2[0] = DX;
					Curve.P2[1] = DY;

					memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
				}

				{
					struct HF_BufferCurve Curve = { 0 };

					Curve.P0[0] = DX;
					Curve.P0[1] = DY;

					Curve.P1[0] = C1X;
					Curve.P1[1] = C1Y;

					Curve.P2[0] = B3X;
					Curve.P2[1] = B3Y;

					memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
				}
			}
			else if (PrevTag == FT_CURVE_TAG_ON)
			{
				struct HF_BufferCurve Curve = { 0 };

				Curve.P0[0] = PrevX;
				Curve.P0[1] = PrevY;

				Curve.P1[0] = 0.5F * (PrevX + CurrX);
				Curve.P1[1] = 0.5F * (PrevY + CurrY);

				Curve.P2[0] = CurrX;
				Curve.P2[1] = CurrY;

				memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
			}
			else
			{
				struct HF_BufferCurve Curve = { 0 };

				Curve.P0[0] = StartX;
				Curve.P0[1] = StartY;

				Curve.P1[0] = PrevX;
				Curve.P1[1] = PrevX;

				Curve.P2[0] = CurrX;
				Curve.P2[1] = CurrY;

				memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
			}

			StartX = CurrX;
			StartY = CurrY;

			ControlX = CurrX;
			ControlY = CurrY;
		}
		else
		{
			if (PrevTag == FT_CURVE_TAG_ON)
			{

			}
			else
			{
				float MidX = 0.5F * (PrevX + CurrX);
				float MidY = 0.5F * (PrevY + CurrY);

				struct HF_BufferCurve Curve = { 0 };

				Curve.P0[0] = StartX;
				Curve.P0[1] = StartY;

				Curve.P1[0] = PrevX;
				Curve.P1[1] = PrevX;

				Curve.P2[0] = MidX;
				Curve.P2[1] = MidY;

				memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));

				StartX = MidX;
				StartY = MidY;

				ControlX = MidX;
				ControlY = MidY;
			}
		}

		PrevX = CurrX;
		PrevY = CurrY;

		PrevTag = CurrTag;
	}

	if (PrevTag == FT_CURVE_TAG_CUBIC)
	{
		float B0X = StartX;
		float B0Y = StartY;

		float B1X = ControlX;
		float B1Y = ControlY;

		float B2X = PrevX;
		float B2Y = PrevY;

		float B3X = FirstX;
		float B3Y = FirstY;

		float C0X = B0X + 0.75F * (B1X - B0X);
		float C0Y = B0Y + 0.75F * (B1Y - B0Y);

		float C1X = B3X + 0.75F * (B2X - B3X);
		float C1Y = B3Y + 0.75F * (B2Y - B3Y);

		float DX = 0.5F * (C0X + C1X);
		float DY = 0.5F * (C0Y + C1Y);

		{
			struct HF_BufferCurve Curve = { 0 };

			Curve.P0[0] = B0X;
			Curve.P0[1] = B0Y;

			Curve.P1[0] = C0X;
			Curve.P1[1] = C0Y;

			Curve.P2[0] = DX;
			Curve.P2[1] = DY;

			memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
		}

		{
			struct HF_BufferCurve Curve = { 0 };

			Curve.P0[0] = DX;
			Curve.P0[1] = DY;

			Curve.P1[0] = C1X;
			Curve.P1[1] = C1Y;

			Curve.P2[0] = B3X;
			Curve.P2[1] = B3Y;

			memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
		}
	}
	else if (PrevTag == FT_CURVE_TAG_ON)
	{
		struct HF_BufferCurve Curve = { 0 };

		Curve.P0[0] = PrevX;
		Curve.P0[1] = PrevY;

		Curve.P1[0] = 0.5F * (PrevX + FirstX);
		Curve.P1[1] = 0.5F * (PrevY + FirstY);

		Curve.P2[0] = FirstX;
		Curve.P2[1] = FirstY;

		memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
	}
	else
	{
		struct HF_BufferCurve Curve = { 0 };

		Curve.P0[0] = StartX;
		Curve.P0[1] = StartY;

		Curve.P1[0] = PrevX;
		Curve.P1[1] = PrevX;

		Curve.P2[0] = FirstX;
		Curve.P2[1] = FirstY;

		memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
	}
}

static void HF_FontUploadBuffers(struct HF_Font *Font)
{
	glBindBuffer(GL_TEXTURE_BUFFER, Font->GlyphBuffer);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(struct HF_BufferGlyph) * Font->BufferGlyphOffset, Font->BufferGlyphs, GL_STATIC_DRAW);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	glBindBuffer(GL_TEXTURE_BUFFER, Font->CurveBuffer);
	glBufferData(GL_TEXTURE_BUFFER, sizeof(struct HF_BufferCurve) * Font->BufferCurveOffset, Font->BufferCurves, GL_STATIC_DRAW);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
}