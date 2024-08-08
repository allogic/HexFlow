#include <stdio.h>
#include <stdlib.h>

#ifdef HF_PLATFORM_WINDOWS

	#include <Windows.h>

#endif

#include <HexFlow/FileSystem.h>
#include <HexFlow/Font.h>
#include <HexFlow/Shader.h>
#include <HexFlow/Matrix.h>
#include <HexFlow/Projection.h>
#include <HexFlow/Graph.h>
#include <HexFlow/List.h>

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

	#pragma section(".vshdr", read)
	#pragma section(".fshdr", read)
	#pragma section(".dfont", read)
	#pragma section(".ucfgr", read)

	__declspec(allocate(".vshdr")) char unsigned const g_VertexShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(".fshdr")) char unsigned const g_FragmentShader[HF_SHADER_SECTION_SIZE];
	__declspec(allocate(".dfont")) char unsigned const g_DefaultFont[HF_FONT_SECTION_SIZE];

	__declspec(allocate(".ucfgr")) struct HF_UserConfig const g_UserConfig;

#else

	#error "Platform not supported"

#endif

static HF_ListEntry m_GraphList = { 0 };

static void HF_PatchSection(char *Image, char const *SectionName, char const *Buffer, long long unsigned Size);

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

			HF_FileSystemReadFile("rb", Argv[0], &ImageBuffer, &ImageSize);
			HF_FileSystemReadFile(Argv[2], Argv[3], &PatchBuffer, &PatchSize);
			HF_PatchSection(ImageBuffer, Argv[4], PatchBuffer, PatchSize);
			HF_FileSystemWriteFile("wb", Argv[5], ImageBuffer, ImageSize);

			free(ImageBuffer);
			free(PatchBuffer);
		}
	}
	else
	{
		double Time = 0.0;
		double TimeDelta = 0.0;
		double TimePrev = 0.0;

		if (glfwInit())
		{
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			glfwWindowHint(GLFW_SAMPLES, 8);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);

			GLFWwindow *Context = glfwCreateWindow(1920, 1080, "HexFlow", 0, 0);

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

					HF_Matrix4 Projection = HF_MATRIX4_IDENTITY;
					HF_Matrix4 View = HF_MATRIX4_IDENTITY;
					HF_Matrix4 Model = HF_MATRIX4_IDENTITY;

					HF_ProjectionOrthographic(-1.0F, 1.0F, -1.0F, 1.0F, 0.001F, 100.0F, Projection);

					HF_Vector3 Translation = { -0.9F, 0.9F, 0.0F };

					HF_Matrix4Translate(Model, Translation);

					struct HF_Shader *FontShader = HF_ShaderAlloc(g_VertexShader, g_FragmentShader);
					struct HF_Font *Font = HF_FontAlloc(g_DefaultFont, sizeof(g_DefaultFont), 0.05F, 0);

					while (!glfwWindowShouldClose(Context))
					{
						Time = glfwGetTime();
						TimeDelta = Time - TimePrev;
						TimePrev = Time;

						glViewport(0, 0, 1920, 1080);
						glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
						glClear(GL_COLOR_BUFFER_BIT);

						HF_FontDrawBegin(Font, FontShader, Projection, View, Model);

						HF_ListEntry *ListEntry = m_GraphList.Flink;
						while (ListEntry != &m_GraphList)
						{
							struct HF_Graph *Graph = (struct HF_Graph*)ListEntry;

							HF_GraphDraw(Graph, Font);

							ListEntry = ListEntry->Flink;
						}

						HF_FontDrawEnd(Font, FontShader);

						glfwSwapBuffers(Context);
						glfwPollEvents();
					}

					HF_FontFree(Font);
					HF_ShaderFree(FontShader);

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

#endif

}

static void HF_WindowResizeCallback(GLFWwindow *Context, int Width, int Height)
{

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

		HF_ListInsertTail(&m_GraphList, HF_GraphListEntry(Graph));
	}
}