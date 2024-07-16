#kind pixel
#version 430 core

layout(location = 1) uniform vec4 u_BaseColor;
layout(location = 2) uniform sampler2D u_BaseColorTex;
layout(location = 3) uniform float u_Metallic;
layout(location = 4) uniform float u_Roughness;
layout(location = 5) uniform sampler2D u_MetallicRoughnessTex;
layout(location = 6) uniform float u_NormalScale;
layout(location = 7) uniform sampler2D u_NormalTex;
layout(location = 8) uniform float u_OcclusionStrength;
layout(location = 9) uniform sampler2D u_OcclusionTex;
layout(location = 10) uniform vec4 u_EmissiveColor;
layout(location = 11) uniform sampler2D u_EmissiveTex;

in Vertex
{
    vec3 WorldPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Binormal;
    vec4 Color;
    vec2 TexCoord0;
} IN;

out vec4 o_Color[4];

void main(void)
{
    o_Color[0] = texture(u_BaseColorTex, IN.TexCoord0) * u_BaseColor;
}