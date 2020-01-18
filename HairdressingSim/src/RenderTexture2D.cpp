#include "RenderTexture2D.h"

RenderTexture2D::RenderTexture2D(int width, int height) : m_width(width), m_height(height)
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glGenFramebuffers(1, &m_framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

	m_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

RenderTexture2D::~RenderTexture2D()
{
	glDeleteFramebuffers(1, &m_framebuffer);
	glDeleteTextures(1, &m_texture);
}

GLuint RenderTexture2D::getTexture()
{
	return m_texture;
}

GLuint RenderTexture2D::getFramebuffer()
{
	return m_framebuffer;
}

bool RenderTexture2D::hasError() const
{
	return m_status != GL_FRAMEBUFFER_COMPLETE;
}

void RenderTexture2D::begin()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glDepthMask(GL_FALSE);
	glViewport(0, 0, m_width, m_height);
}

void RenderTexture2D::end()
{
	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
