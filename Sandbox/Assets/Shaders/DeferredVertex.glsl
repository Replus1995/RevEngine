#kind vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord0;

out Vertex
{
    vec2 TexCoord0;
} OUT;

void main(void)
{
    gl_Position = vec4(a_Position, 1.0);
    OUT.TexCoord0 =  a_TexCoord0;
}