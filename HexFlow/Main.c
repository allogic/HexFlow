#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#ifdef HF_PLATFORM_WINDOWS

	#include <Windows.h>

#endif // HF_PLATFORM_WINDOWS

#include <HexFlow/FileSystem.h>
#include <HexFlow/Gizmos.h>
#include <HexFlow/Font.h>
#include <HexFlow/Shader.h>
#include <HexFlow/Memory.h>
#include <HexFlow/List.h>

#include <HexFlow/Math/Math.h>

#include <HexFlow/NodeGraph/Graph.h>

#include <HexFlow/GLAD/glad.h>

#include <HexFlow/GLFW/glfw3.h>

#define HF_SHADER_SECTION_SIZE 1024 * 4
#define HF_FONT_SECTION_SIZE 1024 * 1024 * 1

#define HF_ALIGN_PAGE_DOWN(VALUE) (((long long unsigned)(VALUE)) & ~((HF_PAGE_SIZE) - 1))
#define HF_ALIGN_PAGE_UP(VALUE) ((((long long unsigned)(VALUE)) + ((HF_PAGE_SIZE) - 1)) & ~((HF_PAGE_SIZE) - 1))

struct HF_UserConfig
{
	void* Dummy;
};

#ifdef HF_PLATFORM_WINDOWS

	#pragma section(".gizvs", read)
	#pragma section(".gizfs", read)

	#pragma section(".fntvs", read)
	#pragma section(".fntfs", read)

	#pragma section(".dfont", read)
	#pragma section(".ucfgr", read)

	__declspec(allocate(".gizvs")) static char unsigned const m_GizmosVertexShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(".gizfs")) static char unsigned const m_GizmosFragmentShader[HF_SHADER_SECTION_SIZE];

	__declspec(allocate(".fntvs")) static char unsigned const m_FontVertexShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(".fntfs")) static char unsigned const m_FontFragmentShader[HF_SHADER_SECTION_SIZE];

	__declspec(allocate(".dfont")) static char unsigned const m_DefaultFont[HF_FONT_SECTION_SIZE];

	__declspec(allocate(".ucfgr")) static struct HF_UserConfig const m_UserConfig;

#else // HF_PLATFORM_WINDOWS

	#error "Platform not supported"

#endif // HF_PLATFORM_WINDOWS

static int m_Width = 1920;
static int m_Height = 1080;

static char m_WindowTitle[64] = "HexFlow";

static HF_ListEntry m_GraphList = { 0 };

static float m_Time = 0.0F;
static float m_TimeDelta = 0.0F;
static float m_TimePrev = 0.0F;

static HF_Matrix4 m_Projection = HF_MATRIX4_IDENTITY;
static HF_Matrix4 m_View = HF_MATRIX4_IDENTITY;
static HF_Matrix4 m_Model = HF_MATRIX4_IDENTITY;

static void HF_PatchSection(char *Image, char const *SectionName, char const *Buffer, long long unsigned Size);

static void HF_UpdateAllGraphs(void);
static void HF_FreeAllGraphs(void);

static void HF_DrawAllGraphsGizmos(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader);
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
		if (strcmp("Patch", Argv[1]) == 0)
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
	}
	else
	{
		HF_ListInitHead(&m_GraphList);

		if (glfwInit())
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			glfwWindowHint(GLFW_SAMPLES, 0);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);

			GLFWwindow *Context = glfwCreateWindow(m_Width, m_Height, m_WindowTitle, 0, 0);

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

					HF_ClipSpacePerspective(HF_DEG_TO_RAD(45.0F), (float)m_Width / m_Height, 0.001F, 100.0F, m_Projection);

					HF_Vector3 Eye = { 0.0F, 0.0F, -90.0F };
					HF_Vector3 Center = { 0.0F, 0.0F, 0.0F };
					HF_Vector3 Up = { 0.0F, 1.0F, 0.0F };

					HF_TransformLookAt(Eye, Center, Up, m_View);

					struct HF_Shader *GizmosShader = HF_ShaderAlloc(m_GizmosVertexShader, m_GizmosFragmentShader);
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

						HF_DrawAllGraphsGizmos(Gizmos, GizmosShader);
						HF_DrawAllGraphsFonts(Font, FontShader);

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
					HF_ShaderFree(GizmosShader);

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

#else // HF_PLATFORM_WINDOWS

	#error "Platform not supported"

#endif // HF_PLATFORM_WINDOWS

}

static void HF_UpdateAllGraphs(void)
{
	HF_ListEntry *ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

		HF_GraphUpdate(Graph);

		ListEntry = ListEntry->Flink;
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

static void HF_DrawAllGraphsGizmos(struct HF_Gizmos *Gizmos, struct HF_Shader *Shader)
{
	HF_GizmosBeginDraw(Gizmos, Shader, m_Projection, m_View, m_Model);

	HF_ListEntry *ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

		HF_GraphDrawGizmos(Graph, Gizmos);

		ListEntry = ListEntry->Flink;
	}

	HF_GizmosEndDraw(Gizmos, Shader);
}

static void HF_DrawAllGraphsFonts(struct HF_Font *Font, struct HF_Shader *Shader)
{
	HF_FontBeginDraw(Font, Shader, m_Projection, m_View, m_Model);

	HF_ListEntry *ListEntry = m_GraphList.Flink;
	while (ListEntry != &m_GraphList)
	{
		struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

		HF_GraphDrawFont(Graph, Font);

		ListEntry = ListEntry->Flink;
	}

	HF_FontEndDraw(Font, Shader);
}

static void HF_WindowResizeCallback(GLFWwindow *Context, int Width, int Height)
{
	m_Width = Width;
	m_Height = Height;

	HF_ClipSpacePerspective(HF_DEG_TO_RAD(45.0F), (float)m_Width / m_Height, 0.001F, 100.0F, m_Projection);

	glViewport(0, 0, m_Width, m_Height);
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

}

static void HF_DropCallback(GLFWwindow *Context, int PathCount, const char **Paths)
{
	for (int PathIndex = 0; PathIndex < PathCount; PathIndex++)
	{
		struct HF_Graph *Graph = HF_GraphAlloc(Paths[PathIndex]);

		HF_ListInsertTail(&m_GraphList, (HF_ListEntry*)Graph);
	}
}