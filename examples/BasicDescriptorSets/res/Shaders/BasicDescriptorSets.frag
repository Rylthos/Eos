#version 450

layout (location = 0) in vec3 i_Colour;

layout (location = 0) out vec4 o_FragColour;

void main()
{
    o_FragColour = vec4(i_Colour, 1.0f);
}
