#version 450

layout (location = 0) in vec3 i_Position;
layout (location = 1) in vec3 i_Colour;

layout (location = 0) out vec3 v_Colour;

layout (set = 0, binding = 0) uniform Data
{
    mat4 projection;
    mat4 view;
    mat4 model;
} u_Data;

void main()
{
    gl_Position = u_Data.projection * u_Data.view * u_Data.model *
        vec4(i_Position, 1.0f);
    v_Colour = i_Colour;
}