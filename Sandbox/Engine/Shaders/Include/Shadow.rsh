#include "/Engine/Shaders/Include/Bindings.rsh"

layout(std140, binding = UBO_BINDING_SHADOW) uniform ShadowUniformBuffer
{
	mat4 LightProjMats[6];
	mat4 LightViewMats[6];
	vec4 LightParams;
} ub_Shadow;
