#version 450

layout (location = 0) in vec3 i_Position;
layout (location = 1) in vec3 i_Colour;

layout (location = 0) out vec3 v_Colour;

void main()
{
    gl_Position = vec4(i_Position, 1.0f);

    v_Colour = i_Colour;
}
