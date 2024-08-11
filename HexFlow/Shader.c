#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <HexFlow/Shader.h>
#include <HexFlow/Memory.h>

#include <HexFlow/GLAD/glad.h>

struct HF_Shader
{
	int unsigned Program;
};

#ifdef HF_DEBUG

static void HF_ShaderCheckCompileStatus(int unsigned Shader);
static void HF_ShaderCheckLinkStatus(int unsigned Program);

#endif // HF_DEBUG

struct HF_Shader* HF_ShaderAlloc(char const *VertexSource, char const *FragmentSource)
{
	struct HF_Shader* Shader = (struct HF_Shader*)HF_MemoryAlloc(sizeof(struct HF_Shader), 0);

	Shader->Program = glCreateProgram();

	int unsigned VertexShader = glCreateShader(GL_VERTEX_SHADER);
	int unsigned FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(VertexShader, 1, &VertexSource, 0);
	glCompileShader(VertexShader);

#ifdef HF_DEBUG

	HF_ShaderCheckCompileStatus(VertexShader);

#endif // HF_DEBUG

	glShaderSource(FragmentShader, 1, &FragmentSource, 0);
	glCompileShader(FragmentShader);

#ifdef HF_DEBUG

	HF_ShaderCheckCompileStatus(FragmentShader);

#endif // HF_DEBUG

	glAttachShader(Shader->Program, VertexShader);
	glAttachShader(Shader->Program, FragmentShader);

	glLinkProgram(Shader->Program);

#ifdef HF_DEBUG

	HF_ShaderCheckLinkStatus(Shader->Program);

#endif // HF_DEBUG

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	return Shader;
}

void HF_ShaderFree(struct HF_Shader* Shader)
{
	glDeleteProgram(Shader->Program);

	HF_MemoryFree(Shader);
}

void HF_ShaderBind(struct HF_Shader *Shader)
{
	glUseProgram(Shader->Program);
}

void HF_ShaderUnbind(struct HF_Shader *Shader)
{
	glUseProgram(0);
}

void HF_ShaderSet1Int32(struct HF_Shader *Shader, char const *Name, int V0)
{
	int Location = glGetUniformLocation(Shader->Program, Name);

	glUniform1i(Location, V0);
}

void HF_ShaderSet2Int32(struct HF_Shader *Shader, char const *Name, int V0, int V1)
{
	int Location = glGetUniformLocation(Shader->Program, Name);

	glUniform2i(Location, V0, V1);
}

void HF_ShaderSet3Int32(struct HF_Shader *Shader, char const *Name, int V0, int V1, int V2)
{
	int Location = glGetUniformLocation(Shader->Program, Name);

	glUniform3i(Location, V0, V1, V2);
}

void HF_ShaderSet4Int32(struct HF_Shader *Shader, char const *Name, int V0, int V1, int V2, int V3)
{
	int Location = glGetUniformLocation(Shader->Program, Name);

	glUniform4i(Location, V0, V1, V2, V3);
}

void HF_ShaderSet1Real32(struct HF_Shader *Shader, char const *Name, float V0)
{
	int Location = glGetUniformLocation(Shader->Program, Name);

	glUniform1f(Location, V0);
}

void HF_ShaderSet2Real32(struct HF_Shader *Shader, char const *Name, float V0, float V1)
{
	int Location = glGetUniformLocation(Shader->Program, Name);

	glUniform2f(Location, V0, V1);
}

void HF_ShaderSet3Real32(struct HF_Shader *Shader, char const *Name, float V0, float V1, float V2)
{
	int Location = glGetUniformLocation(Shader->Program, Name);

	glUniform3f(Location, V0, V1, V2);
}

void HF_ShaderSet4Real32(struct HF_Shader *Shader, char const *Name, float V0, float V1, float V2, float V3)
{
	int Location = glGetUniformLocation(Shader->Program, Name);

	glUniform4f(Location, V0, V1, V2, V3);
}

void HF_ShaderSet1Matrix4(struct HF_Shader *Shader, char const *Name, HF_Matrix4 M)
{
	int Location = glGetUniformLocation(Shader->Program, Name);

	glUniformMatrix4fv(Location, 1, 0, &M[0][0]);
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

#endif // HF_DEBUG