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
	column_major matrix ViewMat;
    column_major matrix ProjMat;
    column_major matrix ViewProjMat;
    column_major matrix InvViewProjMat;
};

struct FStaticMeshUniform
{
    column_major matrix ModelMat;
};

//Default SamplerStates
/*
SamplerState GLinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};
*/

//Texture/Sampler defines
struct FSampler2D
{
	Texture2D Texture;
	SamplerState Sampler;
};
#define SAMPLER2D(_name, _reg) \
    uniform Texture2D _name ## Texture : REGISTER(t, _reg); \
    uniform SamplerState _name ## Sampler : REGISTER(s, _reg); \
    static FSampler2D _name = { _name ## Texture, _name ## Sampler }

float4 Sample2D(FSampler2D _sampler, float2 _coord)
{
	return _sampler.Texture.Sample(_sampler.Sampler, _coord);
}