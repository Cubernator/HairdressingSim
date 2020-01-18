#include "HeadView.h"
#include "HairdressingSim.h"
#include "Head.h"

#include <iostream>
#include <fstream>
#include <algorithm>

HeadView::HeadView(HairdressingSim * parent) : m_parent(parent), m_decalMap(0), m_dragging(false)
{
	// load shaders
	m_skinShader.reset(ShaderProgram::loadProgram("Data\\shaders\\skin_vert.glsl", "", "Data\\shaders\\skin_frag.glsl"));
	if (m_skinShader->hasError()) {
		std::cout << "Skin Shader log:" << std::endl << m_skinShader->getLog() << std::endl;
	}

	m_hairShader.reset(ShaderProgram::loadProgram("Data\\shaders\\hair_vert.glsl", "Data\\shaders\\hair_geom.glsl", "Data\\shaders\\hair_frag.glsl"));
	if (m_hairShader->hasError()) {
		std::cout << "Hair Shader log:" << std::endl << m_hairShader->getLog() << std::endl;
	}

	// initialize variables
	m_eyePos = glm::vec3(0.0f, 0.0f, 15.0f);
	m_view = glm::lookAt(m_eyePos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_hairSegments = 5;
	m_maxHairLength = 1.0f;

	m_hairShininess = 40.0f;
	m_hairSpecInt = 0.1f;

	m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

	m_lightPos = glm::vec3(4.0f, 10.0f, 20.0f);

	m_colorPresets[0] = { getColorFromInt(0x090806) , getColorFromInt(0x090806) };
	m_colorPresets[1] = { getColorFromInt(0xDCD0BA) , getColorFromInt(0xDCD0BA) };
	m_colorPresets[2] = { getColorFromInt(0xE5C8A8) , getColorFromInt(0xE5C8A8) };
	m_colorPresets[3] = { getColorFromInt(0x504444) , getColorFromInt(0x504444) };
	m_colorPresets[4] = { getColorFromInt(0x3B3024) , getColorFromInt(0x3B3024) };
	m_colorPresets[5] = { getColorFromInt(0xB55239) , getColorFromInt(0xB55239) };
	m_colorPresets[6] = { getColorFromInt(0x8D4A43) , getColorFromInt(0x8D4A43) };
	m_colorPresets[7] = { getColorFromInt(0x090806) , getColorFromInt(0x0D2A82) };
	m_colorPresets[8] = { getColorFromInt(0x090806) , getColorFromInt(0x11A62C) };
	m_colorPresets[9] = { getColorFromInt(0x090806) , getColorFromInt(0x4E0F8A) };

	applyColorPreset(0);

	{ // load skin texture

		std::ifstream tfile("Data\\AngelinaDiffuseTex2048.raw", std::ifstream::binary | std::ifstream::ate);
		if (tfile) {
			unsigned int size = (unsigned int)tfile.tellg();
			tfile.seekg(0);

			char* tdata = new char[size];
			tfile.read(tdata, size);
			tfile.close();

			glGenTextures(1, &m_decalMap);
			glBindTexture(GL_TEXTURE_2D, m_decalMap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 2048, 2048, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata);
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);

			delete[] tdata;
		}
	}
}

HeadView::~HeadView()
{
	glDeleteTextures(1, &m_decalMap);
}

void HeadView::applyColorPreset(unsigned int index)
{
	auto& p = m_colorPresets[index];

	m_hairBaseColor = p.first;
	m_hairTipColor = p.second;
}

void HeadView::onKey(int key, int scancode, int action, int mods)
{
	if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
		// set visual max hair length
		if (key == GLFW_KEY_DELETE) {
			m_maxHairLength -= 0.1f;
		} else if (key == GLFW_KEY_INSERT) {
			m_maxHairLength += 0.1f;
		}

		m_maxHairLength = std::min(std::max(m_maxHairLength, 0.1f), 6.0f);
	}

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_1) applyColorPreset(0);
		if (key == GLFW_KEY_2) applyColorPreset(1);
		if (key == GLFW_KEY_3) applyColorPreset(2);
		if (key == GLFW_KEY_4) applyColorPreset(3);
		if (key == GLFW_KEY_5) applyColorPreset(4);
		if (key == GLFW_KEY_6) applyColorPreset(5);
		if (key == GLFW_KEY_7) applyColorPreset(6);
		if (key == GLFW_KEY_8) applyColorPreset(7);
		if (key == GLFW_KEY_9) applyColorPreset(8);
		if (key == GLFW_KEY_0) applyColorPreset(9);
	}
}

void HeadView::onMouseButton(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		m_dragging = action == GLFW_PRESS;
		if (m_dragging) { // start arcball rotation when right mousebutton is pressed down
			m_lastArcvec = getArcvec((float)m_parent->getMouseX(), (float)m_parent->getMouseY());
		}
	}
}

void HeadView::onResized(int width, int height)
{
	// adjust viewport to new window size

	m_winWidth = width;
	m_winHeight = height;

	if (width < height) {
		m_viewport.x = 0;
		m_viewport.y = 0;
		m_viewport.width = width;
		m_viewport.height = height - width;
	} else {
		m_viewport.x = height;
		m_viewport.y = 0;
		m_viewport.width = width - height;
		m_viewport.height = height;
	}

	m_proj = glm::perspective(45.0f, m_viewport.getAspectRatio(), 0.1f, 100.0f);
}

void HeadView::update()
{
	// arcball mouse controls
	if (m_dragging) {
		double mx, my;
		m_parent->getMousePos(mx, my);
		glm::vec3 arcvec = getArcvec((float)mx, (float)my);
		float angle = acosf(std::min(1.0f, glm::dot(m_lastArcvec, arcvec)));

		if (angle >= 0.01f) {
			glm::vec3 axis = glm::cross(m_lastArcvec, arcvec);
			glm::mat3 cam2Obj = glm::inverse(glm::mat3(m_view) * glm::mat3(m_world));
			m_headRot = glm::rotate(m_headRot, angle, cam2Obj * axis);
			m_world = glm::toMat4(m_headRot);
		}

		m_lastArcvec = arcvec;
	}
}

glm::vec3 HeadView::getArcvec(float x, float y)
{
	glm::vec3 result((x - m_viewport.x)/m_viewport.width * 2.0f - 1.0f,
		-(y - (m_winHeight - (m_viewport.y + m_viewport.height)))/m_viewport.height * 2.0f + 1.0f,
		0.0f);
	float l2 = glm::length2(result);
	if (l2 <= 1.0f) {
		result.z = sqrt(1.0f - l2);
	} else {
		result = glm::normalize(result);
	}

	return result;
}

void HeadView::draw() const
{
	glEnable(GL_DEPTH_TEST);
	m_viewport.apply();

	glm::mat4 viewProj = m_proj * m_view;

	// skin pass
	if (!m_skinShader->hasError()) {
		m_skinShader->begin();
		m_skinShader->setUniform("wvp", viewProj * m_world);
		m_skinShader->setUniform("world", m_world);
		m_skinShader->setUniform("lightPos", m_lightPos);
		m_skinShader->setUniform("lightColor", m_lightColor);
		m_skinShader->setTexture("decalMap", m_decalMap);
		m_parent->getHead()->draw();
		m_skinShader->end();
	}

	// hair pass
	if (!m_hairShader->hasError()) {
		m_hairShader->begin();
		m_hairShader->setUniform("world", m_world);
		m_hairShader->setUniform("viewProj", viewProj);
		m_hairShader->setUniform("baseColor", m_hairBaseColor);
		m_hairShader->setUniform("tipColor", m_hairTipColor);
		m_hairShader->setUniform("maxLength", m_maxHairLength);
		m_hairShader->setUniform("segments", m_hairSegments);
		m_hairShader->setUniform("eyePos", m_eyePos);
		m_hairShader->setUniform("lightPos", m_lightPos);
		m_hairShader->setUniform("lightColor", m_lightColor);
		m_hairShader->setUniform("shininess", m_hairShininess);
		m_hairShader->setUniform("specIntensity", m_hairSpecInt);
		m_hairShader->setTexture("hairMap", m_parent->getHairMap()->getTexture());
		m_parent->getHead()->draw();
		m_hairShader->end();
	}

	glDisable(GL_DEPTH_TEST);
}
