#pragma once

#include <GL/glew.h>
#include "math.h"

class GLImage2D
{
public:
	GLImage2D();
	~GLImage2D();

	bool LoadTexture(const std::string& path);

	void Bind() const;

	GLuint GetGLTextureID() const;
	math::vec2 const& GetTextureSize() const;

private:
	GLuint		m_textureID;
	std::string m_textureName;
	math::vec2	m_textureSize;
};

