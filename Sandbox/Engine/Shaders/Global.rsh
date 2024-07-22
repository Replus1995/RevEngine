#define UBO_BINDING_START 0
#define UBO_BINDING(NUMBER) UBO_BINDING_START + NUMBER

#define PI 3.14159265;

layout(std140, binding = UBO_BINDING(0)) uniform CameraUniformBuffer
{
    mat4 ProjMat;
	mat4 ViewMat;
    vec4 Position;
} ub_Camera;

layout(std140, binding = UBO_BINDING(1)) uniform ModelUniformBuffer
{
    mat4 ModelMat;
} ub_Model;