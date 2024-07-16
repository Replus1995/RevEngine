#kind vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Tangent;
layout(location = 3) in vec2 a_TexCoord0;
layout(location = 4) in vec4 a_Color;

layout(std140, binding = 0) uniform SceneData
{
    mat4 u_ProjMat;
	mat4 u_ViewMat;
};
layout(location = 0) uniform mat4 u_ModelMat;

out Vertex
{
    vec3 WorldPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Binormal;
    vec4 Color;
    vec2 TexCoord0;
} OUT;

void main(void)
{
    OUT.Color = a_Color;
    OUT.TexCoord0 = a_TexCoord0;

    mat3 NormalMat = transpose(inverse(mat3(u_ModelMat)));
    vec3 wNormal = normalize(NormalMat * normalize(a_Normal));
    vec3 wTangent =  normalize(NormalMat * normalize(a_Tangent.xyz));

    OUT.Normal = wNormal;
    OUT.Tangent = wTangent;
    OUT.Binormal = cross(wTangent, wNormal) * a_Tangent.w;

    vec4 wPositon = (u_ModelMat * vec4(a_Position, 1.0));
    OUT.WorldPos = wPositon.xyz;

    gl_Position = (u_ProjMat * u_ViewMat) * wPositon;

}