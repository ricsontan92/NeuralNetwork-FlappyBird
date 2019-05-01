#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>

#include <GL/glew.h>

#include "math.h"

class GLShader
{
public:
	GLShader();
	void AddShader(const std::string& path, GLenum shaderType);
	void AddShaderScript(const std::string& script, GLenum shaderType);
	bool GenShaderProgram();
	void UseProgram() const;
	
	void SetMat44(const char* location, const math::mat4& data) const;
	void SetVec4(const char* location, const math::vec4& data) const;
	void SetVec3(const char* location, const math::vec3& data) const;
	void SetVec2(const char* location, const math::vec2& data) const;
	void SetFloat(const char* location, GLfloat data) const;
	void SetInt(const char* location, GLint data) const;

	~GLShader();
private:
	bool CheckShaderProgramLinkStatus(GLuint program_hdl, std::string& diag_msg) const;
	bool CheckShaderCompileStatus(GLuint shader_hdl, std::string& diag_msg) const;
	void GetShaderContents(const std::string& shader, char*& content) const;

	std::unordered_map<std::string, GLuint> m_allShaders;
	GLuint m_shaderProgram;
};

