#define VS_SHADERMODEL vs_5_0
#define PS_SHADERMODEL ps_5_0
#define PI 3.1415926
#define EPSILON 0.00001
struct ScreenUnifom
{
    float Width;
    float Height;
};

struct CameraUniform
{
    float4 ViewPos;
	float4x4 ViewMat;
    float4x4 ProjMat;
    float4x4 InvProjViewMat;
};

struct ModelUniform
{
    float4x4 ModelMat;
};

cbuffer BuildInUB : register(b0, space0)
{
    CameraUniform uCamera;
    ModelUniform uModel;
};
