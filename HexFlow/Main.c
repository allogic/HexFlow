#include <stdio.h>

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
	char Data[0x1000];
};

#ifdef HF_PLATFORM_WINDOWS

	#pragma section(".vshdr", read)
	#pragma section(".fshdr", read)
	#pragma section(".dfont", read)
	#pragma section(".ucfg", read)

	__declspec(allocate(".vshdr")) char unsigned const g_VertexShader[HF_ALIGN_PAGE_UP(HF_SHADER_SECTION_SIZE)];
	__declspec(allocate(".fshdr")) char unsigned const g_FragmentShader[HF_ALIGN_PAGE_UP(HF_SHADER_SECTION_SIZE)];
	__declspec(allocate(".dfont")) char unsigned const g_DefaultFont[HF_ALIGN_PAGE_UP(HF_FONT_SECTION_SIZE)];

	__declspec(allocate(".ucfg")) struct HF_UserConfig const g_UserConfig;

#else

	#error "Platform not supported"

#endif

static void GlfwCursorCallback(GLFWwindow* Context, double X, double Y);

int main()
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
			glfwSetCursorPosCallback(Context, GlfwCursorCallback);
			glfwSwapInterval(0);

			int GLVersion = gladLoadGL();

			if (GLVersion > 0)
			{
				HF_FontInitFreeType();

				while (!glfwWindowShouldClose(Context))
				{
					Time = glfwGetTime();
					TimeDelta = Time - TimePrev;
					TimePrev = Time;

					glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
					glClear(GL_COLOR_BUFFER_BIT);

					glfwMakeContextCurrent(Context);
					glfwSwapBuffers(Context);

					glfwPollEvents();
				}

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

	return 0;
}

static void GlfwCursorCallback(GLFWwindow* Context, double X, double Y)
{

}