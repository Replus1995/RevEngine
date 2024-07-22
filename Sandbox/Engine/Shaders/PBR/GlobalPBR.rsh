#define UL_PBR_START 5
#define UL_PBR(NUMBER) UL_PBR_START + NUMBER


vec3 Schlick_F(vec3 R0, float cosTheta)
{
	return R0 + (vec3(1.0) - R0) * pow(1.0 - cosTheta, 5.0);
}

float GGX_D(vec3 n, vec3 h, float a)
{
    float a2 = a * a;
    float nh = max(dot(n, h), 0);
    float nh2 = nh * nh;
    return a2 / (PI * pow((nh2 * (a2 - 1.0) + 1.0), 2.0));
}

float GGX_G(vec3 n, vec3 v, float a)
{
    float k = pow(a + 1.0, 2.0) / 8.0;
    float nv = max(dot(n, v), 0);
    return nv / (nv * (1 - k) + k);
}