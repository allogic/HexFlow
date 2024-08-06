#ifndef HF_SHADER_H
#define HF_SHADER_H

struct HF_Shader* HF_ShaderAlloc(char const *VertexSource, char const *FragmentSource);
void HF_ShaderFree(struct HF_Shader *Shader);

#endif