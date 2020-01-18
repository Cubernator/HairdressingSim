#version 330

uniform vec3 lightColor;
uniform float shininess;
uniform float specIntensity;

in VertexData {
	vec3 lightVec;
	vec3 viewVec;
	vec3 direction;
	vec3 color;
} input;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec3 L = normalize(input.lightVec);
	vec3 V = normalize(input.viewVec);
	vec3 H = normalize(L + V);
	vec3 D = normalize(input.direction);

	float DdotL = dot(D, L);
	float DdotH = dot(D, H);

	float NdotL = max(0.0, sqrt(1.0 - DdotL*DdotL));
	float NdotH = max(0.0, sqrt(1.0 - DdotH*DdotH));

	vec3 diffuse = NdotL * input.color * lightColor;
	vec3 specular = pow(NdotH, shininess) * lightColor * specIntensity;

	fragColor = vec4(diffuse + specular, 1.0);
}