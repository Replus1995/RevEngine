#kind pixel
#version 430 core

layout(location = 5) uniform vec4 u_BaseColorFactor;
layout(location = 6) uniform sampler2D u_BaseColorTex;
layout(location = 7) uniform float u_Metallic;
layout(location = 8) uniform float u_Roughness;
layout(location = 9) uniform sampler2D u_MetallicRoughnessTex;
layout(location = 10) uniform float u_NormalScale;
layout(location = 11) uniform sampler2D u_NormalTex;
layout(location = 12) uniform float u_OcclusionStrength;
layout(location = 13) uniform sampler2D u_OcclusionTex;
layout(location = 14) uniform vec3 u_EmissiveFactor;
layout(location = 15) uniform sampler2D u_EmissiveTex;

in Vertex
{
    vec3 WorldPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Binormal;
    vec4 Color;
    vec2 TexCoord0;
} IN;

layout(location = 0) out vec4 o_Color;
layout(location = 1) out vec4 o_Normal;
layout(location = 2) out vec4 o_MRO;
layout(location = 3) out vec4 o_Emissive;

void main(void)
{
    //Color
    o_Color = texture(u_BaseColorTex, IN.TexCoord0) * u_BaseColorFactor;

    //Normal
    mat3 TBN =  mat3(normalize(IN.Tangent), normalize(IN.Binormal), normalize(IN.Normal));
    vec3 lNormal = texture(u_NormalTex, IN.TexCoord0).rgb;
    lNormal = normalize((lNormal * 2.0 - 1.0) * vec3(u_NormalScale, u_NormalScale, 1.0f));
    o_Normal = vec4(normalize(TBN * lNormal), 1.0f);

    //MetallicRoughnessOcclusion
    o_MRO.r = u_Metallic * texture(u_MetallicRoughnessTex, IN.TexCoord0).r;
    o_MRO.g = u_Roughness * texture(u_MetallicRoughnessTex, IN.TexCoord0).g;
    o_MRO.b = u_OcclusionStrength * texture(u_OcclusionTex, IN.texCoord0).r;
    o_MRO.a = 0.0f;

    //Emissive
    o_Emissive = vec4(texture(u_EmissiveTex, IN.TexCoord0).rgb * u_EmissiveFactor, 1.0);
}