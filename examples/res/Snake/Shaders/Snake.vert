#version 460

layout (location = 0) in vec3 i_Position;

layout (set = 0, binding = 0) uniform GlobalData
{
    mat4 projection;
    mat4 view;
} u_Global;

layout (std140, set = 0, binding = 1) readonly buffer SegmentData
{
    mat4 model[];
} b_Segment;

void main()
{
    mat4 model = b_Segment.model[gl_InstanceIndex];
    gl_Position = u_Global.projection * u_Global.view * model *
        vec4(i_Position, 1.0f);
}
