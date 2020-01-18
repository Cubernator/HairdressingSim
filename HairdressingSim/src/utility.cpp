#include "utility.h"

#include "GL\glew.h"

#include <iostream>
#include <string>

void Viewport::apply() const
{
	glViewport(x, y, width, height);
}

float Viewport::getAspectRatio() const
{
	return (float)width / height;
}

void printError()
{
	GLenum err = glGetError();

	std::string msg;
	switch (err) {
	case GL_NO_ERROR:
		return;
	case GL_INVALID_ENUM:
		msg = "Invalid enum!";
		break;
	case GL_INVALID_VALUE:
		msg = "Invalid value!";
		break;
	case GL_INVALID_OPERATION:
		msg = "Invalid operation!";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		msg = "Invalid framebuffer operation!";
		break;
	}

	std::cout << msg << std::endl;
}

glm::vec3 getColorFromInt(unsigned int c)
{
	int b = (c >> 0) & 0xff;
	int g = (c >> 8) & 0xff;
	int r = (c >> 16) & 0xff;

	const float w = 1.0f / 255.0f;

	return glm::vec3(r * w, g * w, b * w);
}