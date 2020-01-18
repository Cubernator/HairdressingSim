#version 330

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;

void main()
{
	gl_Position = vec4(a_uv * 2.0 - 1.0, 0.0, 1.0);
}