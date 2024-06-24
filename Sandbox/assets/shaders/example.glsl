#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;

layout(std140, binding = 0) uniform SceneData
{
    mat4 u_ProjMat;
	mat4 u_ViewMat;
};
layout(location = 0) uniform mat4 u_Transform;

void main()
{
	gl_Position = u_ProjMat * u_ViewMat * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 430 core

layout(location = 0) out vec4 color;

layout(location = 1) uniform vec4 u_Color;

void main()
{
	color = u_Color;
}