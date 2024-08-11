#include <HexFlow/Memory.h>
#include <HexFlow/TextEditor.h>

struct HF_TextEditor
{
	void* Dummy;
};

struct HF_TextEditor* HF_TextEditorAlloc()
{
	struct HF_TextEditor *TextEditor = (struct HF_TextEditor*)HF_MemoryAlloc(sizeof(struct HF_TextEditor), 0);



	return TextEditor;
}

void HF_TextEditorFree(struct HF_TextEditor *TextEditor)
{
	HF_MemoryFree(TextEditor);
}