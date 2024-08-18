#include <stdio.h>
#include <string.h>

#include <HexFlow/Font.h>
#include <HexFlow/Gizmos.h>
#include <HexFlow/Memory.h>

#include <HexFlow/Container/Vector.h>

#include <HexFlow/GLFW/glfw3.h>

#include <HexFlow/Math/Vector.h>

#include <HexFlow/NodeGraph/Node.h>

#include <HexFlow/NodeGraph/Nodes/TextEditorNode.h>

struct HF_TextEditorNode
{
	struct HF_Node *Node;
	struct HF_Vector *Lines;

	long long unsigned CursorLine;
	long long unsigned CursorColumn;
};

static void HF_TextEditorNodeAddLine(struct HF_TextEditorNode *TextEditorNode);
static void HF_TextEditorNodeRemoveLine(struct HF_TextEditorNode *TextEditorNode);

static void HF_TextEditorNodeAddChar(struct HF_TextEditorNode *TextEditorNode, char unsigned Character);
static void HF_TextEditorNodeBackspace(struct HF_TextEditorNode *TextEditorNode);

struct HF_TextEditorNode* HF_TextEditorNodeAlloc(struct HF_Node *Node)
{
	struct HF_TextEditorNode *TextEditorNode = (struct HF_TextEditorNode*)HF_MemoryAlloc(sizeof(struct HF_TextEditorNode), 0);

	TextEditorNode->Node = Node;
	TextEditorNode->Lines = HF_VectorAlloc(sizeof(struct HF_Vector*));

	TextEditorNode->CursorLine = 0;
	TextEditorNode->CursorColumn = 0;

	HF_TextEditorNodeAddLine(TextEditorNode);

	return TextEditorNode;
}

void HF_TextEditorNodeFree(struct HF_TextEditorNode *TextEditorNode)
{
	long long unsigned LineCount = HF_VectorCount(TextEditorNode->Lines);

	for (long long unsigned LineIndex = 0; LineIndex < LineCount; LineIndex++)
	{
		struct HF_Vector *Columns = *(struct HF_Vector**)HF_VectorAt(TextEditorNode->Lines, LineIndex);

		HF_VectorFree(Columns);
	}

	HF_VectorFree(TextEditorNode->Lines);

	HF_MemoryFree(TextEditorNode);
}

void HF_TextEditorNodeKeyCallback(struct HF_TextEditorNode *TextEditorNode, int Key, int ScanCode, int Action, int Mods)
{
	switch (Key)
	{
		case GLFW_KEY_ENTER:
		{
			switch (Action)
			{
				case GLFW_PRESS:
				{
					HF_TextEditorNodeAddLine(TextEditorNode);

					break;
				}
			}

			break;
		}
		case GLFW_KEY_BACKSPACE:
		{
			switch (Action)
			{
				case GLFW_PRESS:
				{
					HF_TextEditorNodeBackspace(TextEditorNode);

					break;
				}
			}

			break;
		}
		case GLFW_KEY_SPACE:
		case GLFW_KEY_APOSTROPHE:
		case GLFW_KEY_COMMA:
		case GLFW_KEY_MINUS:
		case GLFW_KEY_PERIOD:
		case GLFW_KEY_SLASH:
		case GLFW_KEY_SEMICOLON:
		case GLFW_KEY_EQUAL:
		case GLFW_KEY_LEFT_BRACKET:
		case GLFW_KEY_BACKSLASH:
		case GLFW_KEY_RIGHT_BRACKET:
		case GLFW_KEY_GRAVE_ACCENT:
		{
			switch (Action)
			{
				case GLFW_PRESS:
				{
					char unsigned Character = (char unsigned)Key;

					HF_TextEditorNodeAddChar(TextEditorNode, Character);

					break;
				}
			}

			break;
		}
		case GLFW_KEY_0:
		case GLFW_KEY_1:
		case GLFW_KEY_2:
		case GLFW_KEY_3:
		case GLFW_KEY_4:
		case GLFW_KEY_5:
		case GLFW_KEY_6:
		case GLFW_KEY_7:
		case GLFW_KEY_8:
		case GLFW_KEY_9:
		{
			switch (Action)
			{
				case GLFW_PRESS:
				{
					char unsigned Character = (char unsigned)Key;

					HF_TextEditorNodeAddChar(TextEditorNode, Character);

					break;
				}
			}

			break;
		}
		case GLFW_KEY_A:
		case GLFW_KEY_B:
		case GLFW_KEY_C:
		case GLFW_KEY_D:
		case GLFW_KEY_E:
		case GLFW_KEY_F:
		case GLFW_KEY_G:
		case GLFW_KEY_H:
		case GLFW_KEY_I:
		case GLFW_KEY_J:
		case GLFW_KEY_K:
		case GLFW_KEY_L:
		case GLFW_KEY_M:
		case GLFW_KEY_N:
		case GLFW_KEY_O:
		case GLFW_KEY_P:
		case GLFW_KEY_Q:
		case GLFW_KEY_R:
		case GLFW_KEY_S:
		case GLFW_KEY_T:
		case GLFW_KEY_U:
		case GLFW_KEY_V:
		case GLFW_KEY_W:
		case GLFW_KEY_X:
		case GLFW_KEY_Y:
		case GLFW_KEY_Z:
		{
			switch (Action)
			{
				case GLFW_PRESS:
				{
					char unsigned Character = (Mods & GLFW_MOD_SHIFT) ? (char unsigned)Key : (char unsigned)Key + 32;

					HF_TextEditorNodeAddChar(TextEditorNode, Character);

					break;
				}
			}
		}
	}
}

void HF_TextEditorNodeUpdate(struct HF_TextEditorNode *TextEditorNode)
{

}

void HF_TextEditorNodeDrawFont(struct HF_TextEditorNode *TextEditorNode, struct HF_Font *Font)
{
	HF_Vector2 ContentSize;

	HF_NodeGetContentSize(TextEditorNode->Node, ContentSize);

	HF_Vector3 ContentPosition;

	HF_NodeGetContentPosition(TextEditorNode->Node, ContentPosition);

	float FontHeight = HF_FontGetHeight(Font);
	long long unsigned LineCount = HF_VectorCount(TextEditorNode->Lines);

	HF_Vector3 LineNumberPosition =
	{
		ContentPosition[0],
		ContentPosition[1],
		ContentPosition[2],
	};

	HF_Vector3 LinePosition =
	{
		ContentPosition[0] + 2.0F,
		ContentPosition[1],
		ContentPosition[2],
	};

	for (long long unsigned LineIndex = 0; LineIndex < LineCount; LineIndex++)
	{
		struct HF_Vector *Columns = *(struct HF_Vector**)HF_VectorAt(TextEditorNode->Lines, LineIndex);

		char *ColumnBuffer = (char*)HF_VectorBuffer(Columns);

		static char LineNumber[32] = { 0 };

		snprintf(LineNumber, sizeof(LineNumber), "%llu", LineIndex);

		HF_FontDrawTextClipped(Font, LineNumberPosition, ContentSize, LineNumber);
		HF_FontDrawTextClipped(Font, LinePosition, ContentSize, ColumnBuffer);

		LineNumberPosition[1] -= FontHeight;
		LinePosition[1] -= FontHeight;
	}
}

void HF_TextEditorNodeDrawLines(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos)
{

}

void HF_TextEditorNodeDrawQuads(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos)
{

}

void HF_TextEditorNodeDrawLineBatch(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos)
{

}

void HF_TextEditorNodeDrawQuadBatch(struct HF_TextEditorNode *TextEditorNode, struct HF_Gizmos *Gizmos)
{

}

static void HF_TextEditorNodeAddLine(struct HF_TextEditorNode *TextEditorNode)
{
	struct HF_Vector *Line = HF_VectorAlloc(sizeof(char unsigned));

	HF_VectorPush(TextEditorNode->Lines, &Line);

	TextEditorNode->CursorLine++;
}

static void HF_TextEditorNodeRemoveLine(struct HF_TextEditorNode *TextEditorNode)
{
	int unsigned NumOfLines = (int unsigned)HF_VectorCount(TextEditorNode->Lines);

	if (NumOfLines > 1)
	{
		struct HF_Vector *Line;

		HF_VectorPop(TextEditorNode->Lines, &Line);

		HF_VectorFree(Line);

		if (TextEditorNode->CursorLine > (NumOfLines - 1))
		{
			TextEditorNode->CursorLine = (NumOfLines - 1);
		}
	}
}

static void HF_TextEditorNodeAddChar(struct HF_TextEditorNode *TextEditorNode, char unsigned Character)
{
	struct HF_Vector *Columns = *(struct HF_Vector**)HF_VectorAt(TextEditorNode->Lines, TextEditorNode->CursorLine - 1);
	
	HF_VectorPush(Columns, &Character);

	TextEditorNode->CursorColumn++;
}

static void HF_TextEditorNodeBackspace(struct HF_TextEditorNode *TextEditorNode)
{
	struct HF_Vector *Columns = *(struct HF_Vector**)HF_VectorAt(TextEditorNode->Lines, TextEditorNode->CursorLine - 1);
	
	if (HF_VectorEmpty(Columns))
	{
		HF_TextEditorNodeRemoveLine(TextEditorNode);
	}
	else
	{
		HF_VectorPop(Columns, 0);

		TextEditorNode->CursorColumn--;
	}
}