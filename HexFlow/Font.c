#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <HexFlow/Font.h>

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

#endif

#define HF_BUFFER_GLYPH_SIZE 0x100
#define HF_BUFFER_CURVE_SIZE 0x1000

#define HF_GLYPH_SIZE 0x400

struct HF_Glyph
{
	int unsigned Index;
	int unsigned BufferIndex;
	int unsigned CurveCount;

	// TODO
	// Important glyph metrics in font units.
	FT_Pos Width, Height;
	FT_Pos BearingX;
	FT_Pos BearingY;
	FT_Pos Advance;
};

struct HF_BufferVertex
{
	float X, Y, U, V;
	int unsigned BufferIndex;
};

struct HF_BufferGlyph
{
	int Start, Count;
};

struct HF_BufferCurve
{
	float X0, Y0, X1, Y1, X2, Y2;
};

struct HF_Font
{
	int unsigned Vao, Vbo, Ebo;
	int unsigned GlyphTexture, CurveTexture;
	int unsigned GlyphBuffer, CurveBuffer;

	float WorldSize;
	float EmSize;

	char unsigned Hinting;

	struct HF_BufferGlyph BufferGlyphs[HF_BUFFER_GLYPH_SIZE];
	struct HF_BufferCurve BufferCurves[HF_BUFFER_CURVE_SIZE];

	int unsigned BufferGlyphOffset;
	int unsigned BufferCurveOffset;

	struct HF_Glyph Glyphs[HF_GLYPH_SIZE];

	int LoadFlags;

	FT_Face Face;
	FT_Kerning_Mode KerningMode;
};

static FT_Library m_Library = 0;

static void HF_FontBuildGlyph(struct HF_Font *Font, int unsigned CharCode, int unsigned GlyphIndex);
static void HF_FontConvertContour(struct HF_Font *Font, short FirstIndex, short LastIndex, float EmSize);

void HF_FontInitFreeType(void)
{
	FT_CHECK(FT_Init_FreeType(&m_Library));
}

void HF_FontDeinitFreeType(void)
{
	FT_CHECK(FT_Done_FreeType(m_Library));
}

struct HF_Font* HF_FontAlloc(char unsigned const *FileBase, long long unsigned FileSize, float WorldSize, char unsigned Hinting)
{
	struct HF_Font* Font = (struct HF_Font*)calloc(1, sizeof(struct HF_Font));

	FT_CHECK(FT_New_Memory_Face(m_Library, FileBase, (int unsigned)FileSize, 0, &Font->Face));

	assert(Font->Face->face_flags & FT_FACE_FLAG_SCALABLE);

	Font->WorldSize = WorldSize;
	Font->Hinting = Hinting;

	if (Hinting)
	{
		Font->LoadFlags = FT_LOAD_NO_BITMAP;
		Font->KerningMode = FT_KERNING_DEFAULT;
		Font->EmSize = WorldSize * 64;

		FT_CHECK(FT_Set_Pixel_Sizes(Font->Face, 0, (int unsigned)ceil(WorldSize)));
	}
	else
	{
		Font->LoadFlags = FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
		Font->KerningMode = FT_KERNING_UNSCALED;
		Font->EmSize = Font->Face->units_per_EM;
	}

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
	glVertexAttribPointer(0, 2, GL_FLOAT, 0, sizeof(struct HF_BufferVertex), (void*)(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, 0, sizeof(struct HF_BufferVertex), (void*)(sizeof(float) * 2));
	glVertexAttribIPointer(2, 1, GL_INT, sizeof(struct HF_BufferVertex), (void*)(sizeof(float) * 4));
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

	//uploadBuffers(); TODO

	glBindTexture(GL_TEXTURE_BUFFER, Font->GlyphTexture);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RG32I, Font->GlyphBuffer);
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

	free(Font);
}

// TODO Cleanup this function!
static void HF_FontBuildGlyph(struct HF_Font *Font, int unsigned CharCode, int unsigned GlyphIndex)
{
	struct HF_BufferGlyph BufferGlyph = { 0 };

	BufferGlyph.Start = Font->BufferCurveOffset;

	short StartIndex = 0;

	for (short ContourIndex = 0; ContourIndex < Font->Face->glyph->outline.n_contours; ContourIndex++)
	{
		HF_FontConvertContour(Font, StartIndex, Font->Face->glyph->outline.contours[ContourIndex], Font->EmSize);

		StartIndex = Font->Face->glyph->outline.contours[ContourIndex] + 1;
	}

	BufferGlyph.Count = Font->BufferCurveOffset - BufferGlyph.Start;

	// TODO: Update Font->BufferCurveOffset

	int unsigned BufferIndex = Font->BufferGlyphOffset;

	memcpy(&Font->BufferGlyphs[Font->BufferGlyphOffset], &BufferGlyph, sizeof(struct HF_BufferGlyph));

	struct HF_Glyph Glyph = { 0 };

	Glyph.Index = GlyphIndex;
	Glyph.BufferIndex = BufferIndex;
	Glyph.CurveCount = BufferGlyph.Count;
	Glyph.Width = Font->Face->glyph->metrics.width;
	Glyph.Height = Font->Face->glyph->metrics.height;
	Glyph.BearingX = Font->Face->glyph->metrics.horiBearingX;
	Glyph.BearingY = Font->Face->glyph->metrics.horiBearingY;
	Glyph.Advance = Font->Face->glyph->metrics.horiAdvance;

	memcpy(&Font->Glyphs[CharCode], &Glyph, sizeof(struct HF_Glyph));

	// TODO: Update Font->BufferGlyphOffset
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

					Curve.X0 = B0X;
					Curve.Y0 = B0Y;

					Curve.X1 = C0X;
					Curve.Y1 = C0Y;

					Curve.X2 = DX;
					Curve.Y2 = DY;

					memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
				}

				{
					struct HF_BufferCurve Curve = { 0 };

					Curve.X0 = DX;
					Curve.Y0 = DY;

					Curve.X1 = C1X;
					Curve.Y1 = C1Y;

					Curve.X2 = B3X;
					Curve.Y2 = B3Y;

					memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
				}
			}
			else if (PrevTag == FT_CURVE_TAG_ON)
			{
				struct HF_BufferCurve Curve = { 0 };

				Curve.X0 = PrevX;
				Curve.Y0 = PrevY;

				Curve.X1 = 0.5F * (PrevX + CurrX);
				Curve.Y1 = 0.5F * (PrevY + CurrY);

				Curve.X2 = CurrX;
				Curve.Y2 = CurrY;

				memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
			}
			else
			{
				struct HF_BufferCurve Curve = { 0 };

				Curve.X0 = StartX;
				Curve.Y0 = StartY;

				Curve.X1 = PrevX;
				Curve.Y1 = PrevX;

				Curve.X2 = CurrX;
				Curve.Y2 = CurrY;

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

				Curve.X0 = StartX;
				Curve.Y0 = StartY;

				Curve.X1 = PrevX;
				Curve.Y1 = PrevX;

				Curve.X2 = MidX;
				Curve.Y2 = MidY;

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

			Curve.X0 = B0X;
			Curve.Y0 = B0Y;

			Curve.X1 = C0X;
			Curve.Y1 = C0Y;

			Curve.X2 = DX;
			Curve.Y2 = DY;

			memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
		}

		{
			struct HF_BufferCurve Curve = { 0 };

			Curve.X0 = DX;
			Curve.Y0 = DY;

			Curve.X1 = C1X;
			Curve.Y1 = C1Y;

			Curve.X2 = B3X;
			Curve.Y2 = B3Y;

			memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
		}
	}
	else if (PrevTag == FT_CURVE_TAG_ON)
	{
		struct HF_BufferCurve Curve = { 0 };

		Curve.X0 = PrevX;
		Curve.Y0 = PrevY;

		Curve.X1 = 0.5F * (PrevX + FirstX);
		Curve.Y1 = 0.5F * (PrevY + FirstY);

		Curve.X2 = FirstX;
		Curve.Y2 = FirstY;

		memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
	}
	else
	{
		struct HF_BufferCurve Curve = { 0 };

		Curve.X0 = StartX;
		Curve.Y0 = StartY;

		Curve.X1 = PrevX;
		Curve.Y1 = PrevX;

		Curve.X2 = FirstX;
		Curve.Y2 = FirstY;

		memcpy(&Font->BufferCurves[Font->BufferCurveOffset++], &Curve, sizeof(struct HF_BufferCurve));
	}
}