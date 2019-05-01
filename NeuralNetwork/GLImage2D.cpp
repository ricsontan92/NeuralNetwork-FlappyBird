#include "SOIL/SOIL.h"
#include "GLImage2D.h"

#include <string>
#include <iostream>

GLImage2D::GLImage2D() : m_textureID(0), m_textureSize(0.f, 0.f)
{
}

GLImage2D::~GLImage2D()
{
	glDeleteTextures(1, &m_textureID);
}

bool GLImage2D::LoadTexture(const std::string& path)
{
	int width;
	int height;
	int channel;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	unsigned char* pData = SOIL_load_image(path.c_str(), &width, &height, &channel, SOIL_LOAD_RGBA);

	if (pData == nullptr) 
	{
		std::string error_msg = "An error occurred while loading " + path + ". Reason: " + SOIL_last_result() + ".";
		std::cout << error_msg << std::endl;
		return false;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);

	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(pData);

	m_textureName = path;

	m_textureSize = math::vec2(static_cast<float>(width), static_cast<float>(height));

	return true;
}

GLuint GLImage2D::GetGLTextureID() const
{
	return m_textureID;
}

math::vec2 const& GLImage2D::GetTextureSize() const
{
	return m_textureSize;
}

void GLImage2D::Bind() const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

