#version 330

uniform vec3 lightColor;

uniform sampler2D decalMap;

in vec2 v_uv;
in vec3 v_normal;
in vec3 v_lightVec;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec3 diffCol = texture(decalMap, v_uv).xyz;

	vec3 N = normalize(v_normal);
	vec3 L = normalize(v_lightVec);

	float NdotL = dot(N, L);
	vec3 finalColor = max(NdotL, 0.0) * diffCol * lightColor;

	fragColor = vec4(finalColor, 1.0);
}