#pragma once
#include <memory>
#include <array>

#include "glm.h"
#include "utility.h"
#include "ShaderProgram.h"

class HairdressingSim;

class HeadView
{
private:
	HairdressingSim * m_parent;
	std::unique_ptr<ShaderProgram> m_skinShader, m_hairShader;

	GLuint m_decalMap;

	Viewport m_viewport;
	int m_winWidth, m_winHeight;
	glm::vec3 m_lastArcvec;

	bool m_dragging;

	glm::quat m_headRot;

	int m_hairSegments;
	float m_maxHairLength, m_hairShininess, m_hairSpecInt;
	glm::vec3 m_eyePos;
	glm::mat4 m_world, m_view, m_proj;
	glm::vec3 m_lightPos;
	glm::vec3 m_hairBaseColor, m_hairTipColor, m_lightColor;

	std::array<std::pair<glm::vec3, glm::vec3>, 10> m_colorPresets;

public:
	HeadView(HairdressingSim * parent);
	~HeadView();

	void onKey(int key, int scancode, int action, int mods);
	void onMouseButton(int button, int action, int mods);
	void onResized(int width, int height);

	void update();
	void draw() const;

	glm::vec3 getArcvec(float x, float y);
	void applyColorPreset(unsigned int index);
};