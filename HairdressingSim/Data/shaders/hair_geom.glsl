#version 330

layout(triangles) in;
layout(line_strip, max_vertices = 42) out;

uniform mat4 viewProj;
uniform vec3 lightPos;
uniform vec3 eyePos;

uniform float maxLength;
uniform int segments;

uniform vec3 baseColor;
uniform vec3 tipColor;

in VertexData {
	vec3 hair;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} input[];

out VertexData {
	vec3 lightVec;
	vec3 viewVec;
	vec3 direction;
	vec3 color;
} output;

void makeHair(float c0, float c1, float c2)
{
	vec3 hair = input[0].hair * c0 + input[1].hair * c1 + input[2].hair * c2;
	float length = maxLength * hair.r;

	if (length >= 0.01) {
		vec3 base = gl_in[0].gl_Position.xyz * c0 + gl_in[1].gl_Position.xyz * c1 + gl_in[2].gl_Position.xyz * c2;
		vec3 normal = input[0].normal * c0 + input[1].normal * c1 + input[2].normal * c2;
		vec3 tangent = input[0].tangent * c0 + input[1].tangent * c1 + input[2].tangent * c2;
		vec3 bitangent = input[0].bitangent * c0 + input[1].bitangent * c1 + input[2].bitangent * c2;

		const float pi = 3.1415927;

		float tanAngle = mix(-pi, pi, hair.g) / segments;
		float bitAngle = mix(-pi, pi, hair.b) / segments;

		float sta = sin(tanAngle);
		float cta = cos(tanAngle);
		mat3 mrt = mat3(
			1.0, 0.0, 0.0,
			0.0, cta, -sta,
			0.0, sta, cta
			);

		float sba = sin(bitAngle);
		float cba = cos(bitAngle);
		mat3 mrb = mat3(
			cba, 0.0, -sba,
			0.0, 1.0, 0.0,
			sba, 0.0, cba
			);

		mat3 mrot = mrb * mrt;
		mat3 mts = mat3(tangent, bitangent, normal);

		vec3 pos = base;
		vec3 dir;

		vec3 d = vec3(0.0, 0.0, 1.0);
		float l = length / segments;

		for (int i = 0; i < (segments+1); ++i) {
			d = mrot * d;
			dir = mts * d;

			gl_Position = viewProj * vec4(pos, 1.0);
			output.lightVec = lightPos - pos;
			output.viewVec = eyePos - pos;
			output.direction = dir;
			output.color = mix(baseColor, tipColor, (1.0 * i) / segments);
			EmitVertex();

			pos = pos + dir * l;
		}

		EndPrimitive();
	}
}

void main()
{
	const float third = 1.0 / 3.0;
	const float ninth = third / 3.0;
	const float ninth4 = ninth * 4.0;
	const float ninth7 = ninth * 7.0;

	makeHair(third, third, third);
	makeHair(ninth, ninth4, ninth4);
	makeHair(ninth4, ninth, ninth4);
	makeHair(ninth4, ninth4, ninth);
	makeHair(ninth7, ninth, ninth);
	makeHair(ninth, ninth7, ninth);
	makeHair(ninth, ninth, ninth7);
}
