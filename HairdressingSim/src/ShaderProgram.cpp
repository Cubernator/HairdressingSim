#include "ShaderProgram.h"

bool ShaderProgram::getTarget(GLenum samplerType, GLenum& target)
{
	switch (samplerType) {
	case GL_SAMPLER_1D:
		target = GL_TEXTURE_1D;
		return true;
	case GL_SAMPLER_2D:
		target = GL_TEXTURE_2D;
		return true;
	case GL_SAMPLER_3D:
		target = GL_TEXTURE_3D;
		return true;
	case GL_SAMPLER_CUBE:
		target = GL_TEXTURE_CUBE_MAP;
		return true;
	case GL_SAMPLER_2D_RECT:
		target = GL_TEXTURE_RECTANGLE;
		return true;
	}

	return false;
}

ShaderProgram * ShaderProgram::loadProgram(const std::string & vFile, const std::string & gFile, const std::string & fFile)
{
	Shader *vShader = nullptr, *gShader = nullptr, *fShader = nullptr;

	if (vFile != "") vShader = new Shader(GL_VERTEX_SHADER, vFile);
	if (gFile != "") gShader = new Shader(GL_GEOMETRY_SHADER, gFile);
	if (fFile != "") fShader = new Shader(GL_FRAGMENT_SHADER, fFile);

	return new ShaderProgram(vShader, gShader, fShader);
}

ShaderProgram::ShaderProgram(Shader * vShader, Shader * gShader, Shader * fShader) : m_program(0), m_vShader(vShader), m_gShader(gShader), m_fShader(fShader)
{
	m_program = glCreateProgram();

	if (m_vShader) glAttachShader(m_program, m_vShader->getID());
	if (m_gShader) glAttachShader(m_program, m_gShader->getID());
	if (m_fShader) glAttachShader(m_program, m_fShader->getID());

	glLinkProgram(m_program);

	GLint status;
	glGetProgramiv(m_program, GL_LINK_STATUS, &status);

	m_hasError = status == GL_FALSE;

	GLint logLength;
	glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);

	GLchar* log = new GLchar[logLength];
	glGetProgramInfoLog(m_program, logLength, &logLength, log);
	m_log.assign(log);
	delete[] log;

	getUniforms();
}

ShaderProgram::~ShaderProgram()
{
	if (m_program) glDeleteProgram(m_program);
}

void ShaderProgram::getUniforms()
{
	GLint uniformCount, maxLength;

	glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &uniformCount);
	glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

	unsigned int unitCounter = 0;

	GLchar* buf = new GLchar[maxLength];
	GLsizei l;
	GLint s;
	GLenum t;

	GLenum texTarget;
	GLuint loc;

	for (int i = 0; i < uniformCount; ++i) {
		glGetActiveUniform(m_program, i, maxLength, &l, &s, &t, buf);
		std::string name(buf);

		loc = glGetUniformLocation(m_program, buf);

		if (getTarget(t, texTarget)) {
			m_textures[name] = ShaderTextureUnit(unitCounter++, texTarget, loc);
		} else {
			m_uniforms[name] = loc;
		}
	}

	delete[] buf;
}

GLuint ShaderProgram::getID() const
{
	return m_program;
}

bool ShaderProgram::hasError() const
{
	return m_hasError;
}

const std::string& ShaderProgram::getLog() const
{
	return m_log;
}

GLint ShaderProgram::getUniformLoc(const std::string & name) const
{
	GLint output = -1;
	getUniformLoc(name, output);
	return output;
}

bool ShaderProgram::getUniformLoc(const std::string & name, GLint & loc) const
{
	if (m_uniforms.count(name)) {
		loc = m_uniforms.at(name);
		return true;
	}

	return false;
}

bool ShaderProgram::getTextureUnit(const std::string & name, ShaderTextureUnit & unit) const
{
	if (m_textures.count(name)) {
		unit = m_textures.at(name);
		return true;
	}

	return false;
}

void ShaderProgram::begin()
{
	if (!m_hasError) glUseProgram(m_program);
}

void ShaderProgram::end()
{
	glUseProgram(0);
}

void ShaderProgram::setUniform(const std::string & name, int value)
{
	GLint loc;
	if (getUniformLoc(name, loc)) {
		glUniform1i(loc, value);
	}
}

void ShaderProgram::setUniform(const std::string & name, unsigned int value)
{
	GLint loc;
	if (getUniformLoc(name, loc)) {
		glUniform1ui(loc, value);
	}
}

void ShaderProgram::setUniform(const std::string & name, float value)
{
	GLint loc;
	if (getUniformLoc(name, loc)) {
		glUniform1f(loc, value);
	}
}

void ShaderProgram::setUniform(const std::string & name, const glm::vec2 & value)
{
	GLint loc;
	if (getUniformLoc(name, loc)) {
		glUniform2fv(loc, 1, &value[0]);
	}
}

void ShaderProgram::setUniform(const std::string & name, const glm::vec3 & value)
{
	GLint loc;
	if (getUniformLoc(name, loc)) {
		glUniform3fv(loc, 1, &value[0]);
	}
}

void ShaderProgram::setUniform(const std::string & name, const glm::vec4 & value)
{
	GLint loc;
	if (getUniformLoc(name, loc)) {
		glUniform4fv(loc, 1, &value[0]);
	}
}

void ShaderProgram::setUniform(const std::string & name, const glm::mat4 & value)
{
	GLint loc;
	if (getUniformLoc(name, loc)) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
	}
}

void ShaderProgram::setTexture(const std::string & name, GLuint tex)
{
	ShaderTextureUnit unit;
	if (getTextureUnit(name, unit)) {
		glUniform1i(unit.location, unit.unit);
		glActiveTexture(GL_TEXTURE0 + unit.unit);
		glBindTexture(unit.target, tex);
	}
}
