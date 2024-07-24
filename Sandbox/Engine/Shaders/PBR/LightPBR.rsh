#include "/Engine/Shaders/Global.rsh"
#include "/Engine/Shaders/PBR/GlobalPBR.rsh"



vec3 Schlick_F(vec3 R0, float cosTheta)
{
	return R0 + (vec3(1.0) - R0) * pow(1.0 - cosTheta, 5.0);
}

float GGX_D(vec3 n, vec3 h, float a)
{
    float a2 = a * a;
    float nh = max(dot(n, h), 0);
    float nh2 = nh * nh;
    return a2 / (PI * pow(nh2 * (a2 - 1.0) + 1.0, 2.0));
}

float GGX_G(vec3 n, vec3 v, float a)
{
    float k = pow(a + 1.0, 2.0) / 8.0;
    float nv = max(dot(n, v), 0);
    return nv / (nv * (1 - k) + k);
}

void ComputeDirectionalLightPBR(
    out vec4 OutDiffuse, out vec4 OutSpecular,
    in UnifiedLight InLight, in vec3 InWorldPos,
    in vec3 InColor, in vec3 InNormal,
    in float InMetallic, in float InRoughness)
{
    vec3 Incident = normalize(InLight.Dir.xyz * -1.0);
    vec3 ViewDir = normalize(ub_Camera.Pos.xyz - InWorldPos);
    vec3 HalfDir = normalize(Incident + ViewDir);

     vec3 Reflective = vec3(0.04); 
    Reflective = mix(Reflective, InColor, InMetallic);
    vec3 F = Schlick_F(Reflective, max(dot(HalfDir, ViewDir), 0));
    float D = GGX_D(InNormal, HalfDir, InRoughness);
    float G = GGX_G(InNormal, ViewDir, InRoughness) * GGX_G(InNormal, Incident, InRoughness);

    vec3 DiffuseBase = (vec3(1.0) - F) * (1.0 - InMetallic) / PI;
    float NI = dot(InNormal, Incident);
    float NV = dot(InNormal, ViewDir);
    vec3 SpecularBase = (F * D * G) / (4 * NI * NV);

    float ShadowFactor = 1.0;
    //Sample ShadowMap
    vec3 Radiance = InLight.Color.rgb * InLight.Intensity * 2.0;

    OutDiffuse.rgb = DiffuseBase * Radiance * NI * ShadowFactor;
    OutSpecular.rgb = SpecularBase * Radiance * NI * ShadowFactor;
}

void ComputeLightPBR(
    out vec4 OutDiffuse, out vec4 OutSpecular,
    in UnifiedLight InLight, in vec3 InWorldPos,
    in vec3 InColor, in vec3 InNormal,
    in float InMetallic, in float InRoughness)
{
    //UnifiedLight Light = ub_ForwardLight.Lights[InLightIndex];
    if(InLight.Type == 1)
    {
        ComputeDirectionalLightPBR(OutDiffuse, OutSpecular,
                                    InLight, InWorldPos,
                                    InColor, InNormal,
                                    InMetallic, InRoughness);
    }
}