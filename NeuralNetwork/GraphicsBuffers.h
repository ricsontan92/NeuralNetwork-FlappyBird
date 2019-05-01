#pragma once

#include "GL/glew.h"
#include <vector>

struct GraphicsBuffers
{
	GLuint m_VAO, m_VBO, m_EBO;

	template<typename T, typename V, unsigned K, unsigned L>
	void SetupVAOs(const T(&vertices)[K], const V(&indices)[L]);

	template<typename T, typename V>
	void SetupVAOs(std::vector<T> const & vertices, const std::vector<V> & indices);
};


template<typename T, typename V, unsigned K, unsigned L>
void GraphicsBuffers::SetupVAOs(const T(&vertices)[K], const V(&indices)[L])
{
	GraphicsBuffers& buffer = *this;

	//Vertex Buffers 
	glGenVertexArrays(1, &buffer.m_VAO);
	glGenBuffers(1, &buffer.m_VBO);
	glGenBuffers(1, &buffer.m_EBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(buffer.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, buffer.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);	// vert position

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));	// vert Texcoords
}

template<typename T, typename V>
void GraphicsBuffers::SetupVAOs(std::vector<T> const & vertices, const std::vector<V> & indices)
{
	GraphicsBuffers& buffer = *this;

	glGenVertexArrays(1, &buffer.m_VAO);
	glGenBuffers(1, &buffer.m_VBO);
	glGenBuffers(1, &buffer.m_EBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(buffer.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, buffer.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
}