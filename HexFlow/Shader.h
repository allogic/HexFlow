#ifndef HF_SHADER_H
#define HF_SHADER_H

void HF_ShaderAlloc(struct HF_Shader *Shader, char const *VertexSource, char const *FragmentSource);
void HF_ShaderFree(struct HF_Shader *Shader);

#endif