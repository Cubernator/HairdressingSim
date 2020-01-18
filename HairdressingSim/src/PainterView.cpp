#include "PainterView.h"

#include <iostream>
#include <algorithm>

PainterView::PainterView(HairdressingSim * parent) : m_parent(parent), m_painting(false), m_showOverlay(true), m_subtractive(false), m_level(false), m_mode(0)
{
	// load shaders
	m_unwrapShader.reset(ShaderProgram::loadProgram("Data\\shaders\\unwrap_vert.glsl", "", "Data\\shaders\\unwrap_frag.glsl"));
	if (m_unwrapShader->hasError()) {
		std::cout << "Unwrap Shader log:" << std::endl << m_unwrapShader->getLog() << std::endl;
	}

	m_spriteShader.reset(ShaderProgram::loadProgram("Data\\shaders\\sprite_vert.glsl", "", "Data\\shaders\\sprite_frag.glsl"));
	if (m_spriteShader->hasError()) {
		std::cout << "Sprite Shader log:" << std::endl << m_spriteShader->getLog() << std::endl;
	}

	m_brushShader.reset(ShaderProgram::loadProgram("Data\\shaders\\brush_vert.glsl", "", "Data\\shaders\\brush_frag.glsl"));
	if (m_brushShader->hasError()) {
		std::cout << "Brush Shader log:" << std::endl << m_brushShader->getLog() << std::endl;
	}

	// create vertex buffer with a single quad

	const unsigned int stride = sizeof(float) * 5;

	float quadVertices[]{
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &m_quadVao);
	glBindVertexArray(m_quadVao);

	glGenBuffers(1, &m_quadVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBuffer);
	glBufferData(GL_ARRAY_BUFFER, stride * 4, quadVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 0)); glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 3)); glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// create helper texture
	m_helperTex = std::make_unique<RenderTexture2D>(2048, 2048);

	m_meshColor = glm::vec3(0.2f, 0.2f, 0.2f);

	m_brushColor = glm::vec3(1.0f, 0.0f, 0.0f);
	m_brushSize = 100.0f;
	m_brushIntensity = 1.0f;
}

PainterView::~PainterView()
{
	glDeleteBuffers(1, &m_quadVBuffer);
	glDeleteVertexArrays(1, &m_quadVao);
}

void PainterView::onKey(int key, int scancode, int action, int mods)
{
	bool down = (action == GLFW_PRESS) || (action == GLFW_REPEAT);
	if (key == GLFW_KEY_LEFT_SHIFT) {
		m_subtractive = down; // subtractive mode
	}

	if (key == GLFW_KEY_LEFT_CONTROL) {
		m_level = down; // levelling mode
	}

	m_mode = m_level ? 2 : (m_subtractive ? 1 : 0);

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_D) {
			m_showOverlay = !m_showOverlay;
		}

		if (key == GLFW_KEY_L) {
			m_brushColor = glm::vec3(1.0f, 0.0f, 0.0f); // paint hair length
		}
		if (key == GLFW_KEY_C) {
			m_brushColor = glm::vec3(0.0f, 1.0f, 0.0f); // paint hair curl
		}
		if (key == GLFW_KEY_T) {
			m_brushColor = glm::vec3(0.0f, 0.0f, 1.0f); // paint hair twist
		}
	}

	if (down) {
		// set brush size
		if (key == GLFW_KEY_END) {
			m_brushSize -= 10.0f;
		} else if (key == GLFW_KEY_HOME) {
			m_brushSize += 10.0f;
		}

		m_brushSize = std::min(std::max(m_brushSize, 10.0f), 400.0f);

		// set brush intensity
		if (key == GLFW_KEY_PAGE_DOWN) {
			m_brushIntensity -= 0.01f;
		} else if (key == GLFW_KEY_PAGE_UP) {
			m_brushIntensity += 0.01f;
		}

		m_brushIntensity = std::min(std::max(m_brushIntensity, 0.0f), 1.0f);
	}
}

void PainterView::onMouseButton(int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		m_painting = action == GLFW_PRESS; // only paint while left mouse button is held down
	}
}

void PainterView::onResized(int width, int height)
{
	// adjust viewport to new window size

	m_viewport.x = 0;
	if (width < height) {
		m_viewport.y = height - width;
		m_viewport.width = width;
		m_viewport.height = width;
	} else {
		m_viewport.y = 0;
		m_viewport.width = height;
		m_viewport.height = height;
	}
}

void PainterView::update()
{
	// get brush position
	double mx, my;
	m_parent->getMousePos(mx, my);

	mx *= 2048.0 / m_viewport.width;
	my *= 2048.0 / m_viewport.height;

	// calculate new brush matrix
	m_brushTransform = glm::ortho(0.0f, 2048.0f, 2048.0f, 0.0f, -1.0f, 1.0f);
	m_brushTransform *= glm::translate(glm::vec3((float)mx, (float)my, 0.0f));
	m_brushTransform *= glm::scale(glm::vec3(m_brushSize, m_brushSize, 1.0f));
}

void PainterView::draw()
{
	glBindVertexArray(m_quadVao);

	if (!m_spriteShader->hasError()) {
		m_spriteShader->begin();
		m_spriteShader->setUniform("transform", glm::mat4());
		m_spriteShader->setTexture("colorMap", m_parent->getHairMap()->getTexture());

		m_viewport.apply();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // draw hairmap onto screen

		m_helperTex->begin();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // copy hairmap into helper texture
		m_helperTex->end();

		m_spriteShader->end();
	}

	if (!m_brushShader->hasError()) {
		m_brushShader->begin();
		m_brushShader->setUniform("transform", m_brushTransform);
		m_brushShader->setUniform("color", m_brushColor);
		m_brushShader->setUniform("intensity", m_brushIntensity);
		m_brushShader->setUniform("mode", m_mode);
		m_brushShader->setTexture("background", m_helperTex->getTexture());

		m_viewport.apply();

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // draw brush onto screen

		if (m_painting) {
			m_parent->getHairMap()->begin();

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // draw brush into hairmap

			m_parent->getHairMap()->end();
		}

		m_brushShader->end();
	}

	glBindVertexArray(0);

	if (m_showOverlay && !m_unwrapShader->hasError()) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		m_viewport.apply();

		m_unwrapShader->begin();
		m_unwrapShader->setUniform("color", m_meshColor);
		m_parent->getHead()->draw(); // draw uv coord overlay
		m_unwrapShader->end();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
