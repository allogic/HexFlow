#include <stdio.h>
#include <stdlib.h>

#ifdef HF_PLATFORM_WINDOWS

	#include <Windows.h>

#else

	#error "Platform not supported"

#endif

#include <HexFlow/Font.h>
#include <HexFlow/Shader.h>

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

	__declspec(allocate(".vshdr")) char unsigned const g_VertexShader[HF_ALIGN_PAGE_UP(HF_SHADER_SECTION_SIZE)];
	__declspec(allocate(".fshdr")) char unsigned const g_FragmentShader[HF_ALIGN_PAGE_UP(HF_SHADER_SECTION_SIZE)];
	__declspec(allocate(".dfont")) char unsigned const g_DefaultFont[HF_ALIGN_PAGE_UP(HF_FONT_SECTION_SIZE)];

	__declspec(allocate(".ucfgr")) struct HF_UserConfig const g_UserConfig;

#else

	#error "Platform not supported"

#endif

static void HF_ReadFile(char const *Mode, char const *FileName, char **Buffer, long long unsigned *Size);
static void HF_WriteFile(char const *Mode, char const *FileName, char const *Buffer, long long unsigned Size);

static void HF_PatchSection(char *Image, char const *SectionName, char const *Buffer, long long unsigned Size);

static void HF_CursorCallback(GLFWwindow *Context, double X, double Y);

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

			HF_ReadFile("rb", Argv[0], &ImageBuffer, &ImageSize);
			HF_ReadFile(Argv[2], Argv[3], &PatchBuffer, &PatchSize);
			HF_PatchSection(ImageBuffer, Argv[4], PatchBuffer, PatchSize);
			HF_WriteFile("wb", Argv[5], ImageBuffer, ImageSize);

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
				glfwSetCursorPosCallback(Context, HF_CursorCallback);
				glfwSwapInterval(0);

				int GLVersion = gladLoadGL();

				if (GLVersion > 0)
				{
					HF_FontInitFreeType();

					struct HF_Shader *Shader = HF_ShaderAlloc(g_VertexShader, g_FragmentShader);
					struct HF_Font *Font = HF_FontAlloc(g_DefaultFont, sizeof(g_DefaultFont), 1.0F, 0);

					while (!glfwWindowShouldClose(Context))
					{
						Time = glfwGetTime();
						TimeDelta = Time - TimePrev;
						TimePrev = Time;

						glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
						glClear(GL_COLOR_BUFFER_BIT);

						glfwSwapBuffers(Context);
						glfwPollEvents();
					}

					HF_FontFree(Font);
					HF_ShaderFree(Shader);

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

static void HF_ReadFile(char const *Mode, char const *FileName, char **Buffer, long long unsigned *Size)
{
	FILE *File = fopen(FileName, Mode);

	fseek(File, 0, SEEK_END);
	*Size = ftell(File);
	*Buffer = calloc((*Size) + 1, sizeof(char unsigned));
	fseek(File, 0, SEEK_SET);

	fread(*Buffer, 1, *Size, File);

	fclose(File);
}

static void HF_WriteFile(char const *Mode, char const *FileName, char const *Buffer, long long unsigned Size)
{
	FILE *File = fopen(FileName, Mode);

	fseek(File, 0, SEEK_SET);

	fwrite(Buffer, 1, Size, File);

	fclose(File);
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

static void HF_CursorCallback(GLFWwindow *Context, double X, double Y)
{

}