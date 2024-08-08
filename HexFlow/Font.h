#ifndef HF_FONT_H
#define HF_FONT_H

typedef float HF_Matrix4[4][4];

void HF_FontInitFreeType(void);
void HF_FontDeinitFreeType(void);

struct HF_Font* HF_FontAlloc(char unsigned const *FileBase, long long unsigned FileSize, float WorldSize, char unsigned Hinting);
void HF_FontFree(struct HF_Font *Font);

void HF_FontDrawBegin(struct HF_Font *Font, struct HF_Shader *FontShader, HF_Matrix4 Projection, HF_Matrix4 View, HF_Matrix4 Model);
void HF_FontDraw(struct HF_Font *Font, float X, float Y, char const *Text);
void HF_FontDrawEnd(struct HF_Font *Font, struct HF_Shader *FontShader);

#endif