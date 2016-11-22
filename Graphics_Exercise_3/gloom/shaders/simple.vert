#version 430 core

layout(location = 0) in vec3 in_vertexPosition;
layout(location = 1) in vec4 in_vertexColor;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec4 out_color;
layout(location = 2) out vec3 out_normal;

uniform layout(location = 0) mat4 u_transformationMatrix;

void main()
{
    gl_Position = u_transformationMatrix * vec4(in_vertexPosition, 1.0f);
	
	out_position = in_vertexPosition;
	out_color = in_vertexColor;
	out_normal = normalize(in_vertexPosition.xyz);
}
