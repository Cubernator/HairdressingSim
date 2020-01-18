#version 330

uniform mat4 wvp;
uniform mat4 world;

uniform vec3 lightPos;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_lightVec;

void main()
{
	gl_Position = wvp * vec4(a_position, 1.0);

	v_uv = a_uv;

	v_normal = (world * vec4(a_normal, 0.0)).xyz;

	vec3 worldPos = (world * vec4(a_position, 1.0)).xyz;
	v_lightVec = lightPos - worldPos;
}