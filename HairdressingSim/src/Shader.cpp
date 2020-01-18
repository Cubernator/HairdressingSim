#include "Shader.h"

#include <fstream>
#include <sstream>

Shader::Shader(GLenum type, const std::string& sourceFile) :  m_shader(0)
{
	using namespace std;

	ifstream sfile(sourceFile);

	if (sfile) {
		std::string source((istreambuf_iterator<char>(sfile)), istreambuf_iterator<char>());
		sfile.close();

		const char* csource = source.c_str();

		m_shader = glCreateShader(type);
		glShaderSource(m_shader, 1, &csource, 0);
		glCompileShader(m_shader);

		GLint status;
		glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);

		GLint logLength;
		glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &logLength);

		GLchar* log = new GLchar[logLength];
		glGetShaderInfoLog(m_shader, logLength, &logLength, log);
		m_log.assign(log);
		delete[] log;
	} else {
		m_hasError = true;
		m_log = "Could not open file: " + sourceFile;
	}
}

Shader::~Shader()
{
	if (m_shader) glDeleteShader(m_shader);
}

GLuint Shader::getID() const
{
	return m_shader;
}

bool Shader::hasError() const
{
	return m_hasError;
}

const std::string& Shader::getLog() const
{
	return m_log;
}
