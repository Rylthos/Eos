#version 450

layout (location = 0) out vec4 o_FragColour;

layout (location = 0) in vec3 v_Colour;

void main()
{
    o_FragColour = vec4(v_Colour, 1.0f);
}
