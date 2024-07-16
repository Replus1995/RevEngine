#kind pixel
#version 430 core

layout(location = 0) uniform sampler2D u_ColorTex;
layout(location = 1) uniform sampler2D u_DiffuseTex;
layout(location = 2) uniform sampler2D u_SpecularTex;
layout(location = 3) uniform sampler2D u_EmissiveTex;

in Vertex
{
    vec2 TexCoord0;
} IN;

out vec4 o_Color;

void main(void)
{
    vec3 color = texture(u_ColorTex, IN.TexCoord0).xyz;
    vec4 diffuse = texture(u_DiffuseTex, IN.texCoord);
    vec3 specular = texture(u_SpecularTex, IN.texCoord).xyz;
    vec3 emissive = texture(u_EmissiveTex, IN.texCoord).xyz;
    //vec3 emissive = pow(texture(u_EmissiveTex, IN.texCoord).rgb, vec3(2.2));

    fragColour.xyz = vec3(0.1) * colour * diffuse.a; // Ambient
    fragColour.xyz += colour * diffuse.rgb; //Diffuse
    fragColour.xyz += specular; // Specular
    fragColour.xyz += emissive; // Emissive
    fragColour.xyz = pow(fragColour.xyz,vec3(1.0/2.2));

    fragColour.a = 1.0;
}