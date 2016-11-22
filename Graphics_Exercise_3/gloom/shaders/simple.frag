#version 430 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec3 in_normal;

layout(location = 0) out vec4 out_fragColor;

uniform layout(location = 1) mat4 u_modelTransformationMatrix;

void main()
{
    // Convert the normal for this fragment into world space
    mat3 normalMatrix = transpose(inverse(mat3(u_modelTransformationMatrix)));
    vec3 normal = normalize(normalMatrix * in_normal);
    
    // Calculate the position of this fragment in world space
    vec3 fragPosition = (u_modelTransformationMatrix * vec4(in_position, 1.0)).rgb;

    // Calculate the cosine of the angle of incidence
    float brightness = dot(normal, -fragPosition) / (length(fragPosition) * length(normal));
    brightness = clamp(brightness, 0.0, 1.0);

	// Make the sun bright by setting brightness to 1 if world position is less than 1 (radius of sun)
	brightness = mix(brightness, 1.0, length(fragPosition) <= 1.0);

    // Calculate out color using brightness
    out_fragColor = vec4(max(brightness, 0.1) * in_color.rgb, 1.0f);
}