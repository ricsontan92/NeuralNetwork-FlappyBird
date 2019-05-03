#pragma once

#include <vector>
#include <unordered_map>

#include "GraphicsBuffers.h"
#include "GLShader.h"
#include "math.h"

class GLImage2D;
class GraphicsManager;

class GLRenderer
{
	struct RenderModel
	{
		GLImage2D&	m_image;
		unsigned	m_cols;
		unsigned	m_rows;
		unsigned	m_currFrame;
		math::vec4  m_tint;
		math::mat4	m_model;
	};

public:
	struct TextureInfo
	{
		math::vec4	m_tint;
		std::string m_textureName;
		unsigned	m_cols;
		unsigned	m_rows;
		unsigned	m_currFrame;
	};

	GLRenderer(GraphicsManager& graphicsMgr);
	~GLRenderer();

	void RenderTextures();

	// utils
	void AddTextureToScene(const TextureInfo & textureInfo, const math::mat4 & model);
	void AddTextureToScene(const TextureInfo & textureInfo, const math::vec3 & position, const math::vec3& scale, float degrees);

private:
	using TextureCache = std::unordered_map<std::string, std::unique_ptr<GLImage2D>>;	
	
	TextureCache				m_textureCache;
	std::vector<RenderModel>	m_models;
	GraphicsBuffers				m_quadBuffer;
	GLShader					m_renderShader;
	GraphicsManager&			m_graphicsMgr;
};