#define UNIFORM_MAX_FORWARD_LIGHTS 32
#define UBO_BINDING_START 0
#define UBO_BINDING(NUMBER) UBO_BINDING_START + NUMBER

const float PI = 3.14159265;

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
    vec3 Pos;
    vec3 Dir;
    int ShadowIndex;
    vec3 Color;
    float Intensity;
    vec4 Params;
};

layout(std140, binding = UBO_BINDING(3)) uniform ForwardLightUniformBuffer
{
	UnifiedLight Lights[UNIFORM_MAX_FORWARD_LIGHTS];
	uint LightCount;
} ub_ForwardLight;