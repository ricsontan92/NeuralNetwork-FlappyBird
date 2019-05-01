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
		GLImage2D&	image;
		math::mat4	model;
	};

public:
	GLRenderer(GraphicsManager& graphicsMgr);
	~GLRenderer();

	void RenderTextures();

	// utils
	void AddTextureToScene(const std::string & textureName, const math::mat4 & model);
	void AddTextureToScene(const std::string & textureName, const math::vec3 & position, const math::vec3& scale, float degrees);

private:
	using TextureCache = std::unordered_map<std::string, std::unique_ptr<GLImage2D>>;	
	
	TextureCache				m_textureCache;
	std::vector<RenderModel>	m_models;
	GraphicsBuffers				m_quadBuffer;
	GLShader					m_renderShader;
	GraphicsManager&			m_graphicsMgr;
};