#version 450

layout (location = 0) out vec4 o_FragColour;
layout (location = 0) in float _; // SegmentPercentage

void main()
{
    o_FragColour = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
