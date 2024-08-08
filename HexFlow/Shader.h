#ifndef HF_SHADER_H
#define HF_SHADER_H

typedef float HF_Matrix4[4][4];

struct HF_Shader* HF_ShaderAlloc(char const *VertexSource, char const *FragmentSource);
void HF_ShaderFree(struct HF_Shader *Shader);

void HF_ShaderBind(struct HF_Shader *Shader);
void HF_ShaderUnbind(struct HF_Shader *Shader);

void HF_ShaderSet1Int32(struct HF_Shader *Shader, char const *Name, int V0);
void HF_ShaderSet2Int32(struct HF_Shader *Shader, char const *Name, int V0, int V1);
void HF_ShaderSet3Int32(struct HF_Shader *Shader, char const *Name, int V0, int V1, int V2);
void HF_ShaderSet4Int32(struct HF_Shader *Shader, char const *Name, int V0, int V1, int V2, int V3);

void HF_ShaderSet1Real32(struct HF_Shader *Shader, char const *Name, float V0);
void HF_ShaderSet2Real32(struct HF_Shader *Shader, char const *Name, float V0, float V1);
void HF_ShaderSet3Real32(struct HF_Shader *Shader, char const *Name, float V0, float V1, float V2);
void HF_ShaderSet4Real32(struct HF_Shader *Shader, char const *Name, float V0, float V1, float V2, float V3);

void HF_ShaderSet1Matrix4(struct HF_Shader *Shader, char const *Name, HF_Matrix4 M);

#endif