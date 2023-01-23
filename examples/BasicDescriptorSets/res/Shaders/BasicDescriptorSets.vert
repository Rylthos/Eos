#version 450

layout (location = 0) in vec3 i_Position;

layout (location = 0) out vec3 v_Colour;

layout (set = 0, binding = 0) uniform Colour
{
    vec4 colour;
} u_Colour;

void main()
{
    gl_Position = vec4(i_Position, 1.0f);

    v_Colour = u_Colour.colour.rgb;
}
