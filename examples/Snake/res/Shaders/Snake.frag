#version 450

layout (location = 0) out vec4 o_FragColour;

layout (location = 0) in float v_SegmentPercentage;

void main()
{
    vec3 startingColour = vec3(0.0, 1.0, 0.0);
    vec3 endColour = vec3(0.0, 0.3, 0.0);
    vec3 colour = mix(startingColour, endColour, v_SegmentPercentage);
    o_FragColour = vec4(colour, 1.0f);
}
