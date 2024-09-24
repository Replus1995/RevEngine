#define VS_SHADERMODEL vs_6_0
#define PS_SHADERMODEL ps_6_0
#define PI 3.1415926
#define EPSILON 0.00001
struct ScreenUniform
{
    uint Width;
    uint Height;
};

struct CameraUniform
{
    float4 ViewPos;
	float4x4 ViewMat;
    float4x4 ProjMat;
    float4x4 ViewProjMat;
    float4x4 InvViewProjMat;
};

struct ModelUniform
{
    float4x4 ModelMat;
    float4x4 MVPMat;
};

cbuffer BuiltInUB : register(b2, space0)
{
	ScreenUniform uScreen;
    CameraUniform uCamera;
    ModelUniform uModel;
};
