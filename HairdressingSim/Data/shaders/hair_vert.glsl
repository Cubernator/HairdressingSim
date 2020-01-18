#version 330

uniform mat4 world;
uniform sampler2D hairMap;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_normal;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;

out VertexData{
	vec3 hair;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} output;

void main()
{
	gl_Position = world * vec4(a_position, 1.0);
	output.hair = texture(hairMap, a_uv).rgb;
	output.normal = (world * vec4(a_normal, 0.0)).xyz;
	output.tangent = (world * vec4(a_tangent, 0.0)).xyz;
	output.bitangent = (world * vec4(a_bitangent, 0.0)).xyz;
}