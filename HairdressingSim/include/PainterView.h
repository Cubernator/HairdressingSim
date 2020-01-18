#pragma once

#include "HairdressingSim.h"
#include "ShaderProgram.h"
#include "RenderTexture2D.h"
#include "utility.h"
#include "glm.h"

#include <memory>

class PainterView
{
private:
	HairdressingSim * m_parent;

	std::unique_ptr<ShaderProgram> m_unwrapShader, m_spriteShader, m_brushShader;
	std::unique_ptr<RenderTexture2D> m_helperTex;

	GLuint m_quadVBuffer, m_quadVao;

	Viewport m_viewport;

	bool m_painting, m_showOverlay, m_subtractive, m_level;
	int m_mode;

	float m_brushSize, m_brushIntensity;
	glm::vec3 m_meshColor, m_brushColor;
	glm::mat4 m_brushTransform;

public:
	PainterView(HairdressingSim * parent);
	~PainterView();

	void onKey(int key, int scancode, int action, int mods);
	void onMouseButton(int button, int action, int mods);
	void onResized(int width, int height);

	void update();
	void draw();
};