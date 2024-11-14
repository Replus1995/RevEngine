#ifndef _LIGHT_RSH_
#define _LIGHT_RSH_

#include "Engine/Shaders/Core/Base.rsh"

#define REV_MAX_DIRECTIONAL_LIGHTS 4
#define REV_MAX_POINT_LIGHTS 16
#define REV_MAX_SPOT_LIGHTS 16

struct FDirectionalLight
{
    float4 Direction;
    float4 Color;
    float Intensity;
    int ShadowIndex;
    uint ShadowCount;
};

struct FDirectionalLightUniform
{
	FDirectionalLight Lights[REV_MAX_DIRECTIONAL_LIGHTS];
	uint Count;
};

//PBR const
static const float3 kDielectric = float3(0.04);
static const float3 kAmbientFactor = float3(0.03);


//copies of UE4
//NdH = dot(Normal, HalfDir)
//NdV = dot(Normal, ViewDir)
//NdL = dot(Normal, LightDir)
float3 F_Schlick(in float3 F0, in float cosTheta)
{
    return F0 + (float3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float D_Blinn(in float roughness, in float NdH)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float n = 2.0 / alpha2 - 2.0;
    return (n + 2.0) / (2.0 * PI) * pow(NdH, n);
}

float D_Beckmann(in float roughness, in float NdH)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdH2 = NdH * NdH;
    return exp((NdH2 - 1.0) / (alpha2 * NdH2)) / (PI * alpha2 * NdH2 * NdH2);
}

float D_GGX(in float roughness, in float NdH)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdH2 = NdH * NdH;
    float denom = NdH2 * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * denom * denom);
}

float G_SchlickGGX(in float roughness, in float NdV, in float NdL)
{
    float r1 = roughness + 1.0;
    float k = (r1 * r1) / 8.0;
    float gv = NdV / (NdV * (1.0 - k) + k);
    float gl = NdL / (NdL * (1.0 - k) + k);
    return gv * gl;
}

float Specular_Phong(in float3 V, in float3 L, in float3 N, in float roughness)
{
    float3 R = reflect(-L, N);
    float RdV = max(dot(R, V), 0.0);
    float shininess = max(2.0 / (roughness * roughness) - 2.0, 0.0);
    return pow(RdV, shininess / 4.0);
}

float Specular_Blinn(in float NdH, in float roughness)
{
    float shininess = max(2.0 / (roughness * roughness) - 2.0, 0.0);
    return pow(NdH, shininess);
}

float Specular_CookTorrance(in float NdL, in float NdV, in float NdH, in float roughness)
{
#ifdef COOK_BLINN
    float D = D_blinn(roughness, NdH);
#elif COOK_BECKMANN
    float D = D_beckmann(roughness, NdH);
#else
    float D = D_GGX(roughness, NdH);
#endif

    float G = G_SchlickGGX(roughness, NdV, NdL);
    return D * G;
}

float3 ComputeLightPBR(
    in float3 BaseColor, in float Roughness, in float Metallic,
    in float3 Normal, in float3 ViewDir, in float3 LightDir,
    in float NdV, in float3 F0, in float Radiance)
{
    float3 HalfDir = normalize(LightDir + ViewDir);
    float NdL = max(dot(Normal, LightDir), 0.0);
    float NdH = max(dot(Normal, HalfDir), 0.0);
    float HdV = max(dot(HalfDir, ViewDir), 0.0);

    float3 F = F_Schlick(F0, HdV);
    float D = D_GGX(Roughness, NdH);
    float G = G_SchlickGGX(Roughness, NdV, NdL);

    float3 kd = lerp(float3(1.0) - F, float3(0.0), Metallic);
    float3 diffuseBRDF = kd * BaseColor / PI;
    float3 specularBRDF = (F * D * G) / max(EPSILON, 4.0 * NdL * NdV);

    return (diffuseBRDF + specularBRDF) * Radiance * NdL;
}

#endif