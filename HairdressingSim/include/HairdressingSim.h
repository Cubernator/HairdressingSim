#pragma once

#include <chrono>
#include <memory>

#include "glm.h"

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "Head.h"
#include "RenderTexture2D.h"

#define START_WIDTH 1500
#define START_HEIGHT 750

typedef std::chrono::duration<float> fsec;

class HeadView;
class PainterView;

class HairdressingSim
{
private:
	static HairdressingSim * s_instance;

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void framebufferCallback(GLFWwindow* window, int width, int height);

	GLFWwindow * m_window;
	int m_error;

	const fsec m_deltaTime;
	fsec m_time;

	double m_mouseX, m_mouseY, m_mouseDeltaX, m_mouseDeltaY;

	std::unique_ptr<Head> m_head;
	std::unique_ptr<RenderTexture2D> m_hairMap;

	std::unique_ptr<HeadView> m_headview;
	std::unique_ptr<PainterView> m_painterview;

	void loadContent();

	void fetchCursorPos();

	void update();
	void render();

	void onKey(int key, int scancode, int action, int mods);
	void onMouseButton(int button, int action, int mods);
	void onResized(int width, int height);

public:
	HairdressingSim();
	~HairdressingSim();

	int getError() const;

	float getTime() const;
	float getDeltaTime() const;

	double getMouseX() const;
	double getMouseY() const;
	void getMousePos(double& x, double& y) const;

	double getMouseDeltaX() const;
	double getMouseDeltaY() const;
	void getMouseDelta(double& x, double& y) const;

	const Head * getHead() const;
	RenderTexture2D * getHairMap();
	const RenderTexture2D * getHairMap() const;

	void run();

	void clearHairMap();
};