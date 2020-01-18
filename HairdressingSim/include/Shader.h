#pragma once

#include <string>

#include "GL\glew.h"

class Shader
{
private:
	GLuint m_shader;
	bool m_hasError;
	std::string m_log;

public:
	Shader(GLenum type, const std::string& sourceFile);
	~Shader();

	GLuint getID() const;

	bool hasError() const;
	const std::string& getLog() const;
};