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
	matrix ViewMat;
    matrix ProjMat;
    matrix ViewProjMat;
    matrix InvViewProjMat;
};

struct FStaticMeshUniform
{
    matrix ModelMat;
};