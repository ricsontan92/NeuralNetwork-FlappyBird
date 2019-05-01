#include "GLShader.h"
#include <cassert>
#include <iostream>

GLShader::GLShader()
{
}

void GLShader::AddShader(const std::string& path, GLenum shaderType)
{
	char* srcBuffer;
	std::string errMsg;

	auto it = m_allShaders.find(path);
	if (it != m_allShaders.end()) glDeleteShader(it->second);

	GLuint& retID = m_allShaders[path];
	retID = glCreateShader(shaderType);		// create shader

	GetShaderContents(path.c_str(), srcBuffer);
	if (srcBuffer == nullptr)
	{
		glDeleteShader(retID);
		m_allShaders.erase(path);
		return;
	}
	glShaderSource(retID, 1, &srcBuffer, nullptr);	// put source into memory
	glCompileShader(retID);							// compile
	free(srcBuffer);									// free to prevent mem leak

	if (!CheckShaderCompileStatus(retID, errMsg))
		std::cerr << errMsg;
}

void GLShader::AddShaderScript(const std::string& script, GLenum shaderType)
{
	GLuint& retID = m_allShaders["__Memory__.vert"];	// load from memory
	retID = glCreateShader(shaderType);				// create shader

	const char* pScript = script.c_str();
	glShaderSource(retID, 1, &pScript, nullptr);	// put source into memory
	glCompileShader(retID);							// compile

	std::string errMsg;
	if (!CheckShaderCompileStatus(retID, errMsg))	// final check
		std::cerr << errMsg;
}

bool GLShader::CheckShaderCompileStatus(GLuint shader_hdl, std::string& diag_msg) const
{
	GLint result;
	glGetShaderiv(shader_hdl, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result) 
	{
		GLint log_len;
		glGetShaderiv(shader_hdl, GL_INFO_LOG_LENGTH, &log_len);
		if (log_len > 0) 
		{
			char *error_log_str = new GLchar[log_len];
			GLsizei written_log_len;
			glGetShaderInfoLog(shader_hdl, log_len, &written_log_len, error_log_str);
			diag_msg = error_log_str;
			delete[] error_log_str;
		}
		return false;
	}
	return true;
}

bool GLShader::CheckShaderProgramLinkStatus(GLuint program_hdl, std::string& diag_msg) const
{
	GLint result;
	glGetProgramiv(program_hdl, GL_LINK_STATUS, &result);
	if (GL_FALSE == result) 
	{
		GLint log_len;
		glGetProgramiv(program_hdl, GL_INFO_LOG_LENGTH, &log_len);
		if (log_len > 0) 
		{
			char *error_log_str = new GLchar[log_len];
			GLsizei written_log_len;
			glGetProgramInfoLog(program_hdl, log_len, &written_log_len, error_log_str);
			diag_msg = error_log_str;
			delete[] error_log_str;
		}
		return false;
	}
	return true;
}

void GLShader::GetShaderContents(const std::string& shader, char*& content) const
{
	FILE *file;
	size_t count = 0;

	if (shader.c_str() != NULL)
	{
		fopen_s(&file, shader.c_str(), "rt");
		if (file != NULL)
		{
			fseek(file, 0, SEEK_END);
			count = ftell(file);
			rewind(file);

			if (count > 0)
			{
				content = (char *)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, file);
				content[count] = '\0';
			}

			fclose(file);
		}
		else
		{
			content = nullptr;
			std::cerr << shader << " not found.";
		}
	}
}

bool GLShader::GenShaderProgram()
{
	m_shaderProgram = glCreateProgram();				// generate the prog

	for (auto& elem : m_allShaders)
		glAttachShader(m_shaderProgram, elem.second);	// attach the shaders
	
	glLinkProgram(m_shaderProgram);	// link the shader
	std::string errMsg;
	if (!CheckShaderProgramLinkStatus(m_shaderProgram, errMsg))
	{
		std::cerr << errMsg;
		return false;
	}

	glValidateProgram(m_shaderProgram);	// validate the shader

	// clear unused shader
	for (auto& elem : m_allShaders)
	{
		glDetachShader(m_shaderProgram, elem.second);
		glDeleteShader(elem.second);
	}

	m_allShaders.clear();
	return true;
}

void GLShader::UseProgram() const
{
	glUseProgram(m_shaderProgram);
}

void GLShader::SetMat44(const char* location, const math::mat4& data) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, location), 1, GL_FALSE, &data.GetTranspose().m44[0][0]);
}

void GLShader::SetInt(const char* location, GLint data) const
{
	glUniform1i(glGetUniformLocation(m_shaderProgram, location), data);
}

void GLShader::SetFloat(const char* location, GLfloat data) const
{
	glUniform1f(glGetUniformLocation(m_shaderProgram, location), data);
}

void GLShader::SetVec4(const char* location, const math::vec4& data) const
{
	glUniform4fv(glGetUniformLocation(m_shaderProgram, location), 1, &data.x);
}

void GLShader::SetVec3(const char* location, const math::vec3& data) const
{
	glUniform3fv(glGetUniformLocation(m_shaderProgram, location), 1, &data.x);
}

void GLShader::SetVec2(const char* location, const math::vec2& data) const
{
	glUniform2fv(glGetUniformLocation(m_shaderProgram, location), 1, &data.x);
}

GLShader::~GLShader()
{
	glDeleteProgram(m_shaderProgram);
	glUseProgram(0);
}
