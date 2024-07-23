#define UNIFORM_MAX_FORWARD_LIGHTS 8
#define UBO_BINDING_START 0
#define UBO_BINDING(NUMBER) UBO_BINDING_START + NUMBER

#define PI 3.1415926

layout(std140, binding = UBO_BINDING(0)) uniform CameraUniformBuffer
{
    mat4 ProjMat;
	mat4 ViewMat;
    vec4 Pos;
} ub_Camera;

layout(std140, binding = UBO_BINDING(1)) uniform ModelUniformBuffer
{
    mat4 ModelMat;
} ub_Model;

layout(std140, binding = UBO_BINDING(2)) uniform ShadowUniformBuffer
{
	mat4 LightProjMats[6];
	mat4 LightViewMats[6];
	uint LightViewCount;
	vec3 LightParams;
} ub_Shadow;

struct UnifiedLight
{
    uint Type;
    int ShadowIndex;
    float Intensity;
    vec4 Pos;
    vec4 Dir;
    vec4 Color;
    vec4 Params;
};

layout(std140, binding = UBO_BINDING(3)) uniform ForwardLightUniformBuffer
{
	uint LightCount;
	UnifiedLight Lights[UNIFORM_MAX_FORWARD_LIGHTS];
} ub_ForwardLight;


float Saturate(in float value)
{
    return clamp(value, 0.0, 1.0);
}