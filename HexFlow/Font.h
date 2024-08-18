#ifndef HF_FONT_H
#define HF_FONT_H

#include <HexFlow/Math/MathTypes.h>

void HF_FontInitFreeType(void);
void HF_FontDeinitFreeType(void);

struct HF_Font* HF_FontAlloc(char unsigned const *FileBase, long long unsigned FileSize, float WorldSize);
void HF_FontFree(struct HF_Font *Font);

float HF_FontGetHeight(struct HF_Font *Font);

void HF_FontBeginDraw(struct HF_Font *Font, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View, HF_Matrix4 Model);
void HF_FontDrawText(struct HF_Font *Font, HF_Vector3 Position, HF_Vector2 Size, char const *Text);
void HF_FontDrawTextClipped(struct HF_Font *Font, HF_Vector3 Position, HF_Vector2 Size, char const *Text);
void HF_FontDrawHex(struct HF_Font *Font, HF_Vector3 Position, HF_Vector2 Size, char unsigned const *Buffer, long long unsigned BufferSize, int unsigned NumOfColumns, float Spacing);
void HF_FontDrawHexClipped(struct HF_Font *Font, HF_Vector3 Position, HF_Vector2 Size, char unsigned const *Buffer, long long unsigned BufferSize, int unsigned NumOfColumns, float Spacing);
void HF_FontEndDraw(struct HF_Font *Font, struct HF_Shader *Shader);

#endif // HF_FONT_H