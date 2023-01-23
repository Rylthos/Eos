#version 450

layout (location = 0) in vec2 v_UV;

layout (location = 0) out vec4 o_FragColour;

layout (set = 0, binding = 0) uniform sampler2D tex;

void main()
{
    o_FragColour = texture(tex, v_UV);
}
