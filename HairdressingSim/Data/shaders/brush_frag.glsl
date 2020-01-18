#version 330

uniform vec3 color;
uniform float intensity;

uniform int mode;

uniform sampler2D background;

in vec2 v_uv;
in vec2 v_uvBack;

layout(location = 0) out vec4 fragColor;

void main()
{
	float alpha = clamp((1.0 - length(v_uv)) * intensity, 0.0, 1.0);
	vec3 brushColor = color * alpha;

	vec3 backColor = texture(background, v_uvBack).rgb;

	switch (mode) {
	case 0: // additive
		backColor += brushColor;
		break;
	case 1: // subtractive
		backColor -= brushColor;
		break;
	case 2: // levelling
		vec3 diff = intensity * color - backColor;
		backColor += diff * brushColor;
		break;
	}

	fragColor = vec4(backColor, 1.0);
}