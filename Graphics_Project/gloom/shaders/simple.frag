#version 430 core

layout(location = 0) in vec4 in_color;

layout(location = 0) out vec4 out_fragColor;

uniform layout(location = 1) bool u_selected;

void main()
{
    // Calculate out color using brightness
    out_fragColor = vec4(mix(in_color.rgb, vec3(0.75, 0.75, 0.25), u_selected), 1.0f);
}