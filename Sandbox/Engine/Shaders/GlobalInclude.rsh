#define UBO_BINDING_START 0
#define UBO_BINDING(NUMBER) UBO_BINDING_START + NUMBER

#define UL_PBR_START 5
#define UL_PBR(NUMBER) UL_PBR_START + NUMBER

layout(std140, binding = UBO_BINDING(0)) uniform SceneUniformBuffer
{
    mat4 u_ProjMat;
	mat4 u_ViewMat;
} SceneUB;