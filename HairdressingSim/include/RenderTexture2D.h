#pragma once

#include "GL\glew.h"

class RenderTexture2D
{
private:
	GLuint m_texture, m_framebuffer;
	GLenum m_status;

	int m_width, m_height;

public:
	RenderTexture2D(int width, int height);
	~RenderTexture2D();

	GLuint getTexture();
	GLuint getFramebuffer();

	bool hasError() const;

	void begin();
	void end();
};