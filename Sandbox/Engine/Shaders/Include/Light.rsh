#include "/Engine/Shaders/Include/Global.rsh"

#define UNIFORM_MAX_FORWARD_LIGHTS 32
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

layout(std140, binding = UBO_BINDING_LIGHTS) uniform ForwardLightUniformBuffer
{
	uint LightCount;
	UnifiedLight Lights[UNIFORM_MAX_FORWARD_LIGHTS];
} ub_ForwardLight;

#define LIGHT_KIND_DIRECTIONAL 1
#define LIGHT_KIND_SPOT 2
#define LIGHT_KIND_POINT 3

//PBR const
const vec3 kDielectric = vec3(0.04);
const vec3 kAmbientFactor = vec3(0.3);

//copies of UE4
//NdH = dot(Normal, HalfDir)
//NdV = dot(Normal, ViewDir)
//NdL = dot(Normal, LightDir)
vec3 F_Schlick(in vec3 F0, in float cosTheta)
{
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
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

float Specular_Phong(in vec3 V, in vec3 L, in vec3 N, in float roughness)
{
    vec3 R = reflect(-L, N);
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

//Out.xyz : FinalLightDir
//Out.w : Radiance
vec4 PreComputeLight(in vec3 InWorldPos, in UnifiedLight InLightData)
{
    vec4 Result = vec4(0.0);
    vec3 InLightDir = InLightData.Dir.xyz;
    vec3 InLightPos = InLightData.Pos.xyz;
    if(InLightData.Type == LIGHT_KIND_DIRECTIONAL)
    {
        Result.xyz = -InLightDir;
        Result.w = InLightData.Intensity;
    }
    else
    {
        float radius = InLightData.Params.x;
        vec3 distVec = InLightPos - InWorldPos;
        float atten = 1.0 - clamp(length(distVec) / radius, 0.0, 1.0);
        Result.xyz = normalize(distVec);
        Result.w = InLightData.Intensity * atten;
        if(InLightData.Type == LIGHT_KIND_SPOT)
        {
            float spotAngle = cos(InLightData.Params.y);
            float pixelAngle = dot(InLightDir, normalize(InWorldPos - InLightPos));
            float regionFactor = pixelAngle >= spotAngle ? 1.0 : 0.0;
            Result.w *= regionFactor;
        }
    }
    return Result;
}