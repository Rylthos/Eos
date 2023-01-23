#version 460

layout (location = 0) in vec3 i_Position;

layout (location = 0) out float v_SegmentPercentage;

struct SegmentData
{
    mat4 model;
    uint currentSegment;
};

layout (set = 0, binding = 0) uniform GlobalData
{
    mat4 projection;
    mat4 view;
    uint totalSegments;
} u_Global;

layout (std140, set = 0, binding = 1) readonly buffer Segments
{
    SegmentData segments[];
} b_Segment;

void main()
{
    SegmentData segment = b_Segment.segments[gl_InstanceIndex];
    mat4 model = segment.model;
    gl_Position = u_Global.projection * u_Global.view * model *
        vec4(i_Position, 1.0f);

    v_SegmentPercentage = float(segment.currentSegment) / float(u_Global.totalSegments);
}
