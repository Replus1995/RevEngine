#ifndef _DEFINES_RSH_
#define _DEFINES_RSH_

#define VS_SHADERMODEL vs_6_0
#define PS_SHADERMODEL ps_6_0
#define PI 3.1415926
#define EPSILON 0.00001

#define REV_MAX_DIRECTIONAL_LIGHTS 4
#define REV_MAX_POINT_LIGHTS 16
#define REV_MAX_SPOT_LIGHTS 16

#define REV_MAX_SHADOW_VIEWS 6

#define REGISTER(_type, _reg) register( _type[_reg] )

#endif