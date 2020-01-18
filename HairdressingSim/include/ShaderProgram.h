#pragma once

#include <memory>
#include <unordered_map>

#include "Shader.h"
#include "glm.h"

struct ShaderTextureUnit
{
	unsigned int unit;
	GLenum target;
	GLint location;

	ShaderTextureUnit() = default;
	ShaderTextureUnit(unsigned int unit, GLenum target, GLint location) : unit(unit), target(target), location(location) { }
};

class ShaderProgram
{
private:
	static bool getTarget(GLenum samplerType, GLenum& target);

	GLuint m_program;
	std::unique_ptr<Shader> m_vShader, m_gShader, m_fShader;

	std::unordered_map<std::string, GLint> m_uniforms;
	std::unordered_map<std::string, ShaderTextureUnit> m_textures;

	bool m_hasError;
	std::string m_log;

	void getUniforms();

public:
	static ShaderProgram * loadProgram(const std::string& vFile, const std::string& gFile, const std::string& fFile);

	ShaderProgram(Shader * vShader, Shader * gShader, Shader * fShader);
	~ShaderProgram();

	GLuint getID() const;

	bool hasError() const;
	const std::string& getLog() const;

	GLint getUniformLoc(const std::string& name) const;
	bool getUniformLoc(const std::string& name, GLint& loc) const;

	bool getTextureUnit(const std::string& name, ShaderTextureUnit& unit) const;

	void begin();
	void end();

	void setUniform(const std::string& name, int value);
	void setUniform(const std::string& name, unsigned int value);
	void setUniform(const std::string& name, float value);
	void setUniform(const std::string& name, const glm::vec2& value);
	void setUniform(const std::string& name, const glm::vec3& value);
	void setUniform(const std::string& name, const glm::vec4& value);
	void setUniform(const std::string& name, const glm::mat4& value);

	void setTexture(const std::string& name, GLuint tex);
};