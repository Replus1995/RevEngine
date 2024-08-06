#define PI 3.1415926
#define EPSILON 0.00001

#include "/Engine/Shaders/Include/Bindings.rsh"

layout(std140, binding = UBO_BINDING_CAMERA) uniform CameraUniformBuffer
{
    mat4 ProjMat;
	mat4 ViewMat;
    mat4 InvProjViewMat;
    vec4 Pos;
} ub_Camera;

layout(std140, binding = UBO_BINDING_SCENE) uniform SceneUniformBuffer
{
    float ViewWidth;
    float ViewHeight;
} ub_Scene;

layout(std140, binding = UBO_BINDING_MODEL) uniform ModelUniformBuffer
{
    mat4 ModelMat;
} ub_Model;