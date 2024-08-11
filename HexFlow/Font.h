#ifndef HF_FONT_H
#define HF_FONT_H

typedef float HF_Vector2[2];
typedef float HF_Vector3[3];

typedef float HF_Matrix4[4][4];

void HF_FontInitFreeType(void);
void HF_FontDeinitFreeType(void);

struct HF_Font* HF_FontAlloc(char unsigned const *FileBase, long long unsigned FileSize, float WorldSize);
void HF_FontFree(struct HF_Font *Font);

void HF_FontBeginDraw(struct HF_Font *Font, struct HF_Shader *Shader, HF_Matrix4 Projection, HF_Matrix4 View, HF_Matrix4 Model);
void HF_FontDrawText(struct HF_Font *Font, HF_Vector3 Position, HF_Vector2 Size, char const *Text);
void HF_FontDrawHex(struct HF_Font *Font, HF_Vector3 Position, HF_Vector2 Size, char unsigned const *Buffer, long long unsigned BufferSize, int unsigned NumOfColumns, float Spacing);
void HF_FontEndDraw(struct HF_Font *Font, struct HF_Shader *Shader);

#endif // HF_FONT_H