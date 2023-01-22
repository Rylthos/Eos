#version 450

layout (location = 0) in vec3 i_Position;
layout (location = 1) in vec2 i_UV;

layout (location = 0) out vec2 v_UV;

void main()
{
    gl_Position = vec4(i_Position, 1.0f);

    v_UV = i_UV;
}
