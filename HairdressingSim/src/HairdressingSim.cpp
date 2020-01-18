#include "HairdressingSim.h"
#include "HeadView.h"
#include "PainterView.h"

#include <iostream>

HairdressingSim* HairdressingSim::s_instance = nullptr;

void HairdressingSim::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	s_instance->onKey(key, scancode, action, mods);
}

void HairdressingSim::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
	s_instance->onMouseButton(button, action, mods);
}

void HairdressingSim::framebufferCallback(GLFWwindow * window, int width, int height)
{
	s_instance->onResized(width, height);
}

HairdressingSim::HairdressingSim() : m_window(nullptr), m_error(0), m_deltaTime(1.f / 60.f), m_time(0.f), m_mouseX(0.0), m_mouseY(0.0), m_mouseDeltaX(0.0), m_mouseDeltaY(0.0)
{
	s_instance = this;

	glfwSetErrorCallback([] (int err, const char* msg) {
		std::cout << msg << std::endl;
	});

	// initialize GLFW
	if (!glfwInit()) {
		m_error = -1;
		return;
	}

	// create window
	glfwWindowHint(GLFW_SAMPLES, 4);
	m_window = glfwCreateWindow(START_WIDTH, START_HEIGHT, "Hairdressing Simulator 2016", NULL, NULL);
	if (!m_window) {
		m_error = -1;
		return;
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	// set callbacks
	glfwSetKeyCallback(m_window, HairdressingSim::keyCallback);
	glfwSetMouseButtonCallback(m_window, HairdressingSim::mouseButtonCallback);
	glfwSetFramebufferSizeCallback(m_window, HairdressingSim::framebufferCallback);

	// initialize GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		m_error = -1;
		return;
	}

	loadContent();
	fetchCursorPos();

	onResized(START_WIDTH, START_HEIGHT);
}

HairdressingSim::~HairdressingSim()
{
	m_painterview.reset();
	m_headview.reset();
	m_head.reset();
	m_hairMap.reset();

	glfwTerminate();
}

int HairdressingSim::getError() const
{
	return m_error;
}

float HairdressingSim::getTime() const { return m_time.count(); }
float HairdressingSim::getDeltaTime() const { return m_deltaTime.count(); };

double HairdressingSim::getMouseX() const { return m_mouseX; };
double HairdressingSim::getMouseY() const { return m_mouseY; }
void HairdressingSim::getMousePos(double& x, double& y) const
{
	x = m_mouseX;
	y = m_mouseY;
}

double HairdressingSim::getMouseDeltaX() const { return m_mouseDeltaX; }
double HairdressingSim::getMouseDeltaY() const { return m_mouseDeltaY; }
void HairdressingSim::getMouseDelta(double& x, double& y) const
{
	x = m_mouseDeltaX;
	y = m_mouseDeltaY;
}

const Head * HairdressingSim::getHead() const
{
	return m_head.get();
}

RenderTexture2D * HairdressingSim::getHairMap()
{
	return m_hairMap.get();
}

const RenderTexture2D * HairdressingSim::getHairMap() const
{
	return m_hairMap.get();
}

void HairdressingSim::loadContent()
{
	m_hairMap = std::make_unique<RenderTexture2D>(2048, 2048);

	clearHairMap();

	m_head = std::make_unique<Head>();
	m_headview = std::make_unique<HeadView>(this);
	m_painterview = std::make_unique<PainterView>(this);
}

void HairdressingSim::fetchCursorPos()
{
	double x, y;
	glfwGetCursorPos(m_window, &x, &y);

	m_mouseDeltaX = x - m_mouseX;
	m_mouseDeltaY = y - m_mouseY;

	m_mouseX = x;
	m_mouseY = y;
}

void HairdressingSim::run()
{
	if (m_error) return;

	using namespace std::chrono;
	system_clock::time_point currentTime = system_clock::now(), newTime;
	fsec frameTime(0.0f), accumulator(0.0f);
	const fsec maxFrameTime(0.25f);

	// main loop
	while (!glfwWindowShouldClose(m_window)) {

		newTime = system_clock::now();

		frameTime = newTime - currentTime;
		if (frameTime > maxFrameTime) frameTime = maxFrameTime;
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= m_deltaTime) {
			m_time += m_deltaTime;
			accumulator -= m_deltaTime;

			update();
		}

		render();
	}
}

void HairdressingSim::clearHairMap()
{
	unsigned char clearCol[]{ 0, 127, 127, 255 };
	glClearTexImage(m_hairMap->getTexture(), 0, GL_RGBA, GL_UNSIGNED_BYTE, clearCol);
}

void HairdressingSim::update()
{
	glfwPollEvents();
	fetchCursorPos();

	m_headview->update();
	m_painterview->update();
}

void HairdressingSim::render()
{
	glClearColor(0.1f, 0.4f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_headview->draw();
	m_painterview->draw();
	
	glfwSwapBuffers(m_window);
}

void HairdressingSim::onKey(int key, int scancode, int action, int mods)
{
	m_headview->onKey(key, scancode, action, mods);
	m_painterview->onKey(key, scancode, action, mods);

	if (key == GLFW_KEY_R && action == GLFW_PRESS) {
		clearHairMap(); // reset hair
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwDestroyWindow(m_window); // exit application
	}
}

void HairdressingSim::onMouseButton(int button, int action, int mods)
{
	m_headview->onMouseButton(button, action, mods);
	m_painterview->onMouseButton(button, action, mods);
}

void HairdressingSim::onResized(int width, int height)
{
	m_headview->onResized(width, height);
	m_painterview->onResized(width, height);
}
