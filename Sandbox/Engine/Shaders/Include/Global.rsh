#define VS_SHADERMODEL vs_6_0
#define PS_SHADERMODEL ps_6_0
#define PI 3.1415926
#define EPSILON 0.00001

struct FRect2D
{
    uint X;
    uint Y;
    uint Width;
    uint Height;
};
struct FSceneUniform
{
    FRect2D ViewExtent;
    float4 ViewPos;
	float4x4 ViewMat;
    float4x4 ProjMat;
    float4x4 ViewProjMat;
    float4x4 InvViewProjMat;
};

struct FModelUniform
{
    float4x4 ModelMat;
};

cbuffer SceneUB : register(b2, space0) //update every frame
{
	FSceneUniform uScene;
};

cbuffer ModelUB : register(b3, space0) //update every model
{
    FModelUniform uModel;
};

