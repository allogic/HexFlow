#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#ifdef HF_PLATFORM_WINDOWS

	#include <Windows.h>

#endif // HF_PLATFORM_WINDOWS

#include <HexFlow/FileSystem.h>
#include <HexFlow/Font.h>
#include <HexFlow/Gizmos.h>
#include <HexFlow/List.h>
#include <HexFlow/Memory.h>
#include <HexFlow/Shader.h>

#include <HexFlow/Math/Math.h>

#include <HexFlow/NodeGraph/Graph.h>

#include <HexFlow/NodeGraph/Nodes/TextEditorNode.h>

#include <HexFlow/GLAD/glad.h>

#include <HexFlow/GLFW/glfw3.h>

#define HF_SHADER_SECTION_SIZE 1024 * 4
#define HF_FONT_SECTION_SIZE 1024 * 1024 * 1

#define HF_WINDOW_TITLE_SIZE 64

#define HF_ALIGN_PAGE_DOWN(VALUE) (((long long unsigned)(VALUE)) & ~((HF_PAGE_SIZE) - 1))
#define HF_ALIGN_PAGE_UP(VALUE) ((((long long unsigned)(VALUE)) + ((HF_PAGE_SIZE) - 1)) & ~((HF_PAGE_SIZE) - 1))

#define HF_GIZMOS_LINE_VERTEX_SHADER_SECTION_NAME ".glvs"
#define HF_GIZMOS_LINE_FRAGMENT_SHADER_SECTION_NAME ".glfs"
#define HF_GIZMOS_QUAD_VERTEX_SHADER_SECTION_NAME ".gqvs"
#define HF_GIZMOS_QUAD_FRAGMENT_SHADER_SECTION_NAME ".gqfs"
#define HF_GIZMOS_LINE_BATCH_VERTEX_SHADER_SECTION_NAME ".glbvs"
#define HF_GIZMOS_LINE_BATCH_FRAGMENT_SHADER_SECTION_NAME ".glbfs"
#define HF_GIZMOS_QUAD_BATCH_VERTEX_SHADER_SECTION_NAME ".gqbvs"
#define HF_GIZMOS_QUAD_BATCH_FRAGMENT_SHADER_SECTION_NAME ".gqbfs"
#define HF_FONT_VERTEX_SHADER_SECTION_NAME ".fntvs"
#define HF_FONT_FRAGMENT_SHADER_SECTION_NAME ".fntfs"
#define HF_DEFAULT_FONT_SECTION_NAME ".dfont"
#define HF_USER_CONFIG_SECTION_NAME ".ucfgr"

struct HF_UserConfig
{
	int unsigned WindowWidth;
	int unsigned WindowHeight;

	char const WindowTitle[HF_WINDOW_TITLE_SIZE];

	char unsigned WindowAntiAliasing;
};

#ifdef HF_PLATFORM_WINDOWS

	#pragma section(HF_GIZMOS_LINE_VERTEX_SHADER_SECTION_NAME, read)
	#pragma section(HF_GIZMOS_LINE_FRAGMENT_SHADER_SECTION_NAME, read)
	#pragma section(HF_GIZMOS_QUAD_VERTEX_SHADER_SECTION_NAME, read)
	#pragma section(HF_GIZMOS_QUAD_FRAGMENT_SHADER_SECTION_NAME, read)
	#pragma section(HF_GIZMOS_LINE_BATCH_VERTEX_SHADER_SECTION_NAME, read)
	#pragma section(HF_GIZMOS_LINE_BATCH_FRAGMENT_SHADER_SECTION_NAME, read)
	#pragma section(HF_GIZMOS_QUAD_BATCH_VERTEX_SHADER_SECTION_NAME, read)
	#pragma section(HF_GIZMOS_QUAD_BATCH_FRAGMENT_SHADER_SECTION_NAME, read)
	#pragma section(HF_FONT_VERTEX_SHADER_SECTION_NAME, read)
	#pragma section(HF_FONT_FRAGMENT_SHADER_SECTION_NAME, read)
	#pragma section(HF_DEFAULT_FONT_SECTION_NAME, read)
	#pragma section(HF_USER_CONFIG_SECTION_NAME, read)

	__declspec(allocate(HF_GIZMOS_LINE_VERTEX_SHADER_SECTION_NAME)) static char unsigned const m_GizmosLineVertexShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(HF_GIZMOS_LINE_FRAGMENT_SHADER_SECTION_NAME)) static char unsigned const m_GizmosLineFragmentShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(HF_GIZMOS_QUAD_VERTEX_SHADER_SECTION_NAME)) static char unsigned const m_GizmosQuadVertexShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(HF_GIZMOS_QUAD_FRAGMENT_SHADER_SECTION_NAME)) static char unsigned const m_GizmosQuadFragmentShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(HF_GIZMOS_LINE_BATCH_VERTEX_SHADER_SECTION_NAME)) static char unsigned const m_GizmosLineBatchVertexShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(HF_GIZMOS_LINE_BATCH_FRAGMENT_SHADER_SECTION_NAME)) static char unsigned const m_GizmosLineBatchFragmentShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(HF_GIZMOS_QUAD_BATCH_VERTEX_SHADER_SECTION_NAME)) static char unsigned const m_GizmosQuadBatchVertexShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(HF_GIZMOS_QUAD_BATCH_FRAGMENT_SHADER_SECTION_NAME)) static char unsigned const m_GizmosQuadBatchFragmentShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(HF_FONT_VERTEX_SHADER_SECTION_NAME)) static char unsigned const m_FontVertexShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(HF_FONT_FRAGMENT_SHADER_SECTION_NAME)) static char unsigned const m_FontFragmentShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(HF_DEFAULT_FONT_SECTION_NAME)) static char unsigned const m_DefaultFont[HF_FONT_SECTION_SIZE];
	__declspec(allocate(HF_USER_CONFIG_SECTION_NAME)) static struct HF_UserConfig const m_UserConfig;

#else

	#error "Platform not supported"

#endif // HF_PLATFORM_WINDOWS

static int unsigned m_WindowWidth = 0;
static int unsigned m_WindowHeight = 0;
static char m_WindowTitle[HF_WINDOW_TITLE_SIZE] = { 0 };
static int unsigned m_WindowAntiAliasing = 0;

static HF_ListEntry m_GraphList = { 0 };

static float m_Time = 0.0F;
static float m_TimeDelta = 0.0F;
static float m_TimePrev = 0.0F;

static HF_Matrix4 m_Projection = HF_MATRIX4_IDENTITY;
static HF_Matrix4 m_View = HF_MATRIX4_IDENTITY;
static HF_Matrix4 m_Model = HF_MATRIX4_IDENTITY;

static void HF_PatchSection(char *Image, char const *SectionName, char const *Buffer, long long unsigned Size);

static void HF_LoadUserConfig(void);
static void HF_UpdateProjection(void);

static void HF_UpdateAllGraphs(void);
static void HF_FreeAllGraphs(void);

static void HF_DrawAllGraphsGizmos(struct HF_Gizmos *Gizmos, struct HF_Shader *LineShader, struct HF_Shader *QuadShader, struct HF_Shader *LineBatchShader, struct HF_Shader *QuadBatchShader);
static void HF_DrawAllGraphsFonts(struct HF_Font *Font, struct HF_Shader *Shader);

static void HF_WindowResizeCallback(GLFWwindow *Context, int Width, int Height);
static void HF_CursorPositionCallback(GLFWwindow *Context, double X, double Y);
static void HF_MouseButtonCallback(GLFWwindow *Context, int Button, int Action, int Mods);
static void HF_ScrollCallback(GLFWwindow *Context, double X, double Y);
static void HF_KeyCallback(GLFWwindow *Context, int Key, int ScanCode, int Action, int Mods);
static void HF_DropCallback(GLFWwindow *Context, int PathCount, const char **Paths);

int main(int Argc, char** Argv)
{
	if (Argc > 1)
	{
		if (strcmp("PatchFile", Argv[1]) == 0)
		{
			char* ImageBuffer = 0;
			char* PatchBuffer = 0;

			long long unsigned ImageSize = 0;
			long long unsigned PatchSize = 0;

			HF_FileSystemReadBinary(Argv[0], &ImageBuffer, &ImageSize);

			if (strcmp("ReadText", Argv[2]) == 0)
			{
				HF_FileSystemReadText(Argv[3], &PatchBuffer, &PatchSize);
			}
			else if (strcmp("ReadBinary", Argv[2]) == 0)
			{
				HF_FileSystemReadBinary(Argv[3], &PatchBuffer, &PatchSize);
			}

			HF_PatchSection(ImageBuffer, Argv[4], PatchBuffer, PatchSize);
			HF_FileSystemWriteBinary(Argv[5], ImageBuffer, ImageSize);

			HF_MemoryFree(ImageBuffer);
			HF_MemoryFree(PatchBuffer);
		}
		else if (strcmp("PatchDefaultUserConfig", Argv[1]) == 0)
		{
			char* ImageBuffer = 0;

			long long unsigned ImageSize = 0;

			HF_FileSystemReadBinary(Argv[0], &ImageBuffer, &ImageSize);

			struct HF_UserConfig DefaultUserConfig;

			DefaultUserConfig.WindowWidth = 1920;
			DefaultUserConfig.WindowHeight = 1080;
			strcpy((char*)DefaultUserConfig.WindowTitle, "HexFlow");
			DefaultUserConfig.WindowAntiAliasing = 16;

			HF_PatchSection(ImageBuffer, HF_USER_CONFIG_SECTION_NAME, (char const*)&DefaultUserConfig, sizeof(DefaultUserConfig));
			HF_FileSystemWriteBinary(Argv[2], ImageBuffer, ImageSize);

			HF_MemoryFree(ImageBuffer);
		}
	}
	else
	{
		HF_LoadUserConfig();

		HF_ListInitHead(&m_GraphList);

		if (glfwInit())
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			glfwWindowHint(GLFW_SAMPLES, m_WindowAntiAliasing);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);

			GLFWwindow *Context = glfwCreateWindow(m_WindowWidth, m_WindowHeight, m_WindowTitle, 0, 0);

			if (Context)
			{
				glfwMakeContextCurrent(Context);
				glfwSwapInterval(0);
				glfwSetWindowSizeCallback(Context, HF_WindowResizeCallback);
				glfwSetCursorPosCallback(Context, HF_CursorPositionCallback);
				glfwSetMouseButtonCallback(Context, HF_MouseButtonCallback);
				glfwSetScrollCallback(Context, HF_ScrollCallback);
				glfwSetKeyCallback(Context, HF_KeyCallback);
				glfwSetDropCallback(Context, HF_DropCallback);

				int GLVersion = gladLoadGL();

				if (GLVersion > 0)
				{
					HF_FontInitFreeType();

					HF_UpdateProjection();

					//HF_Vector3 Eye = { 0.0F, 0.0F, -20.0F };
					//HF_Vector3 Center = { 0.0F, 0.0F, 0.0F };
					//HF_Vector3 Up = { 0.0F, 1.0F, 0.0F };

					//HF_TransformLookAt(Eye, Center, Up, m_View);

					struct HF_Shader *GizmosLineShader = HF_ShaderAlloc(m_GizmosLineVertexShader, m_GizmosLineFragmentShader);
					struct HF_Shader *GizmosQuadShader = HF_ShaderAlloc(m_GizmosQuadVertexShader, m_GizmosQuadFragmentShader);
					struct HF_Shader *GizmosLineBatchShader = HF_ShaderAlloc(m_GizmosLineBatchVertexShader, m_GizmosLineBatchFragmentShader);
					struct HF_Shader *GizmosQuadBatchShader = HF_ShaderAlloc(m_GizmosQuadBatchVertexShader, m_GizmosQuadBatchFragmentShader);
					struct HF_Shader *FontShader = HF_ShaderAlloc(m_FontVertexShader, m_FontFragmentShader);

					struct HF_Gizmos *Gizmos = HF_GizmosAlloc();
					struct HF_Font *Font = HF_FontAlloc(m_DefaultFont, sizeof(m_DefaultFont), 1.0F);

					// TODO
					{
						struct HF_Graph* Graph = HF_GraphAlloc("C:\\Users\\mialb\\Downloads\\HexFlow\\Shaders\\FontFragmentShader.glsl");
						HF_ListInsertTail(&m_GraphList, (HF_ListEntry*)Graph);
					}

					while (!glfwWindowShouldClose(Context))
					{
						m_Time = (float)glfwGetTime();
						m_TimeDelta = m_Time - m_TimePrev;
						m_TimePrev = m_Time;

						glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

						glEnable(GL_CULL_FACE);
						glEnable(GL_DEPTH_TEST);
						glEnable(GL_BLEND);

						glDepthFunc(GL_LESS);
						glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

						glCullFace(GL_BACK);
						glFrontFace(GL_CCW);

						HF_UpdateAllGraphs();

						HF_DrawAllGraphsFonts(Font, FontShader);
						HF_DrawAllGraphsGizmos(Gizmos, GizmosLineShader, GizmosQuadShader, GizmosLineBatchShader, GizmosQuadBatchShader);

						glDisable(GL_BLEND);
						glDisable(GL_DEPTH_TEST);
						glDisable(GL_CULL_FACE);

						glfwSwapBuffers(Context);
						glfwPollEvents();
					}

					HF_FreeAllGraphs();

					HF_FontFree(Font);
					HF_GizmosFree(Gizmos);

					HF_ShaderFree(FontShader);
					HF_ShaderFree(GizmosLineShader);
					HF_ShaderFree(GizmosQuadShader);
					HF_ShaderFree(GizmosLineBatchShader);
					HF_ShaderFree(GizmosQuadBatchShader);

					HF_FontDeinitFreeType();
				}
				else
				{
					printf("Failed loading GL\n");
				}

				glfwDestroyWindow(Context);
				glfwTerminate();
			}
			else
			{
				printf("Failed creating window\n");
			}
		}
		else
		{
			printf("Failed initializing GLFW\n");
		}
	}

	assert(g_Allocated == 0);

	return 0;
}

static void HF_PatchSection(char *Image, char const *SectionName, char const *Buffer, long long unsigned Size)
{

#ifdef HF_PLATFORM_WINDOWS

	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)Image;
	PIMAGE_NT_HEADERS64 NTHeaders = (PIMAGE_NT_HEADERS64)((PBYTE)Image + DosHeader->e_lfanew);
	PIMAGE_FILE_HEADER FileHeader = &NTHeaders->FileHeader;
	PIMAGE_SECTION_HEADER SectionHeaders = (PIMAGE_SECTION_HEADER)((PBYTE)NTHeaders + sizeof(IMAGE_NT_HEADERS64));

	for (WORD SectionIndex = 0; SectionIndex < FileHeader->NumberOfSections; SectionIndex++)
	{
		PIMAGE_SECTION_HEADER SectionHeader = &SectionHeaders[SectionIndex];

		if (strcmp(SectionName, SectionHeader->Name) == 0)
		{
			memcpy(Image + SectionHeader->PointerToRawData, Buffer, Size);
		}
	}

#else

	#error "Platform not supported"

#endif // HF_PLATFORM_WINDOWS

}

static void HF_LoadUserConfig(void)
{
	m_WindowWidth = m_UserConfig.WindowWidth;
	m_WindowHeight = m_UserConfig.WindowHeight;
	strcpy(m_WindowTitle, m_UserConfig.WindowTitle);
	m_WindowAntiAliasing = m_UserConfig.WindowAntiAliasing;
}

static void HF_UpdateProjection(void)
{
	float Scale = 15.0F;
	float AspectRatio = (float)m_WindowWidth / m_WindowHeight;

	float HalfWidth = ((float)m_WindowWidth / 2.0F) / Scale;
	float HalfHeight = ((float)m_WindowHeight / 2.0F) / Scale;

	HF_ClipSpaceOrthographic(-HalfWidth, HalfWidth, -HalfHeight, HalfHeight, 0.001F, 100.0F, m_Projection);

	HF_Vector3 Position = { -HalfWidth + (100.0F / Scale), HalfHeight - (50.0F / Scale), 0.0F };

	HF_Matrix4SetPosition(m_View, Position);
}

static void HF_UpdateAllGraphs(void)
{
	HF_ListEntry *ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_GraphUpdate(Graph);
	}
}

static void HF_FreeAllGraphs(void)
{
	HF_ListEntry *ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_GraphFree(Graph);
	}
}

static void HF_DrawAllGraphsGizmos(struct HF_Gizmos *Gizmos, struct HF_Shader *LineShader, struct HF_Shader *QuadShader, struct HF_Shader *LineBatchShader, struct HF_Shader *QuadBatchShader)
{
	HF_ListEntry *ListEntry = 0;

	HF_GizmosBeginLines(Gizmos, LineShader, m_Projection, m_View, m_Model);

	ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph* Graph = (struct HF_Graph*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_GraphDrawLines(Graph, Gizmos);
	}

	HF_GizmosEndLines(Gizmos, LineShader);

	HF_GizmosBeginQuads(Gizmos, QuadShader, m_Projection, m_View, m_Model);

	ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_GraphDrawQuads(Graph, Gizmos);
	}

	HF_GizmosEndQuads(Gizmos, QuadShader);

	HF_GizmosBeginLineBatch(Gizmos, LineBatchShader, m_Projection, m_View);

	ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_GraphDrawLineBatch(Graph, Gizmos);
	}

	HF_GizmosDrawLineBatch(Gizmos);
	HF_GizmosEndLineBatch(Gizmos, LineBatchShader);

	HF_GizmosBeginQuadBatch(Gizmos, QuadBatchShader, m_Projection, m_View);

	ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_GraphDrawQuadBatch(Graph, Gizmos);		
	}

	HF_GizmosDrawQuadBatch(Gizmos);
	HF_GizmosEndQuadBatch(Gizmos, QuadBatchShader);
}

static void HF_DrawAllGraphsFonts(struct HF_Font *Font, struct HF_Shader *Shader)
{
	HF_FontBeginDraw(Font, Shader, m_Projection, m_View, m_Model);

	HF_ListEntry *ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

		ListEntry = ListEntry->Flink;

		HF_GraphDrawFont(Graph, Font);
	}

	HF_FontEndDraw(Font, Shader);
}

static void HF_WindowResizeCallback(GLFWwindow *Context, int Width, int Height)
{
	m_WindowWidth = (int unsigned)Width;
	m_WindowHeight = (int unsigned)Height;

	glViewport(0, 0, Width, Height);

	HF_UpdateProjection();
}

static void HF_CursorPositionCallback(GLFWwindow *Context, double X, double Y)
{

}

static void HF_MouseButtonCallback(GLFWwindow *Context, int Button, int Action, int Mods)
{

}

static void HF_ScrollCallback(GLFWwindow *Context, double X, double Y)
{

}

static void HF_KeyCallback(GLFWwindow *Context, int Key, int ScanCode, int Action, int Mods)
{
	HF_ListEntry *ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

		ListEntry = ListEntry->Flink;

		struct HF_TextEditorNode *TextEditor = HF_GraphGetTextEditor(Graph);

		HF_TextEditorNodeKeyCallback(TextEditor, Key, ScanCode, Action, Mods);
	}
}

static void HF_DropCallback(GLFWwindow *Context, int PathCount, const char **Paths)
{
	for (int PathIndex = 0; PathIndex < PathCount; PathIndex++)
	{
		struct HF_Graph *Graph = HF_GraphAlloc(Paths[PathIndex]);

		HF_ListInsertTail(&m_GraphList, (HF_ListEntry*)Graph);
	}
}