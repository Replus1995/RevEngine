#kind vertex
#version 430 core

layout(location = 0) in vec3 a_Position;

layout(location = 0) uniform mat4 u_ModelMat;
layout(location = 1) uniform mat4 u_ShadowProjMat;
layout(location = 2) uniform mat4 u_ShadowViewMat;

void main()
{
	gl_Position = u_ShadowProjMat * u_ShadowViewMat * u_ModelMat * vec4(a_Position, 1.0);
}