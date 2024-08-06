#ifndef HF_FONT_H
#define HF_FONT_H

void HF_FontInitFreeType(void);
void HF_FontDeinitFreeType(void);

void HF_FontAlloc(struct HF_Font *Font, char const *FilePath, float WorldSize, char unsigned Hinting);
void HF_FontFree(struct HF_Font *Font);

#endif