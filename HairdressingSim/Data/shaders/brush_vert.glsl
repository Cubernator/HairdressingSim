#version 330

uniform mat4 transform;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;

out vec2 v_uv;
out vec2 v_uvBack;

void main()
{
	vec4 pos = transform * vec4(a_position, 1.0);

	gl_Position = pos;
	v_uv = a_uv * 2.0 - 1.0;
	v_uvBack = (pos.xy + 1.0) * 0.5;
}