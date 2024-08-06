#ifndef HF_FONT_H
#define HF_FONT_H

void HF_FontInitFreeType(void);
void HF_FontDeinitFreeType(void);

struct HF_Font* HF_FontAlloc(char unsigned const *FileBase, long long unsigned FileSize, float WorldSize, char unsigned Hinting);
void HF_FontFree(struct HF_Font *Font);

#endif