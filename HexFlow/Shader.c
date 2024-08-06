#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <HexFlow/Shader.h>

#include <HexFlow/GLAD/glad.h>

struct HF_Shader
{
	int unsigned Program;
};

#ifdef HF_DEBUG

static void HF_ShaderCheckCompileStatus(int unsigned Shader);
static void HF_ShaderCheckLinkStatus(int unsigned Program);

#endif

struct HF_Shader* HF_ShaderAlloc(char const *VertexSource, char const *FragmentSource)
{
	struct HF_Shader* Shader = (struct HF_Shader*)calloc(1, sizeof(struct HF_Shader));

	Shader->Program = glCreateProgram();

	int unsigned VertexShader = glCreateShader(GL_VERTEX_SHADER);
	int unsigned FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(VertexShader, 1, &VertexSource, 0);
	glCompileShader(VertexShader);

#ifdef HF_DEBUG

	HF_ShaderCheckCompileStatus(VertexShader);

#endif

	glShaderSource(FragmentShader, 1, &FragmentSource, 0);
	glCompileShader(FragmentShader);

#ifdef HF_DEBUG

	HF_ShaderCheckCompileStatus(FragmentShader);

#endif

	glAttachShader(Shader->Program, VertexShader);
	glAttachShader(Shader->Program, FragmentShader);

	glLinkProgram(Shader->Program);

#ifdef HF_DEBUG

	HF_ShaderCheckLinkStatus(Shader->Program);

#endif

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	return Shader;
}

void HF_ShaderFree(struct HF_Shader* Shader)
{
	glDeleteProgram(Shader->Program);

	free(Shader);
}

#ifdef HF_DEBUG

static void HF_ShaderCheckCompileStatus(int unsigned Shader)
{
	int CompileStatus = 0;
	int InfoLogLength = 0;

	static char Log[0x400] = { 0 };

	glGetShaderiv(Shader, GL_COMPILE_STATUS, &CompileStatus);

	if (CompileStatus == 0)
	{
		glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0)
		{
			InfoLogLength = min(InfoLogLength, sizeof(Log));

			glGetShaderInfoLog(Shader, InfoLogLength, &InfoLogLength, Log);

			printf(Log);
		}
	}
}

static void HF_ShaderCheckLinkStatus(int unsigned Program)
{
	int CompileStatus = 0;
	int InfoLogLength = 0;

	static char Log[0x400] = { 0 };

	glGetProgramiv(Program, GL_LINK_STATUS, &CompileStatus);

	if (CompileStatus == 0)
	{
		glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0)
		{
			InfoLogLength = min(InfoLogLength, sizeof(Log));

			glGetProgramInfoLog(Program, InfoLogLength, &InfoLogLength, &Log[0]);

			printf(Log);
		}
	}
}

#endif