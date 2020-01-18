#pragma once

#include "GL\glew.h"

class Head
{
private:
	GLuint m_vbuffer, m_ibuffer, m_vao;
	unsigned int m_indexCount;

public:
	Head();
	~Head();

	void draw() const;
};