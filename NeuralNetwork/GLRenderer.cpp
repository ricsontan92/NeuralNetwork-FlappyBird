#include "GLRenderer.h"

#include "Camera.h"
#include "GLImage2D.h"
#include "GraphicsManager.h"

GLRenderer::GLRenderer(GraphicsManager& graphicsMgr) : 
		m_graphicsMgr(graphicsMgr)
{
	m_renderShader.AddShader("Assets/BasicShader.frag", GL_FRAGMENT_SHADER);
	m_renderShader.AddShader("Assets/BasicShader.vert", GL_VERTEX_SHADER);
	m_renderShader.GenShaderProgram();
	m_quadBuffer.SetupVAOs(gQuadVertices, gQuadIndices);
}

GLRenderer::~GLRenderer()
{

}

void GLRenderer::AddTextureToScene(const std::string & texturePath, const math::mat4 & model)
{
	std::unique_ptr<GLImage2D>& pTexture = m_textureCache[texturePath];

	if (pTexture == nullptr)
	{
		pTexture = std::make_unique<GLImage2D>();
		if (!pTexture->LoadTexture(texturePath))
		{
			return;
		}
	}

	RenderModel renderModel{ *pTexture ,model };
	m_models.emplace_back(std::move(renderModel));
}

void GLRenderer::AddTextureToScene(const std::string & textureName, const math::vec3 & position, const math::vec3& scale, float degrees)
{
	math::mat4 transform =	math::mat4::Translate(position) *
							math::mat4::Rotate2D(degrees) *
							math::mat4::Scale(scale);
	AddTextureToScene(textureName, transform);
}

void GLRenderer::RenderTextures()
{
	m_graphicsMgr.EnableAlphaBlend();

	// render...
	m_renderShader.UseProgram();
	m_renderShader.SetMat44("projView", m_graphicsMgr.GetMainCamera().Get2DProjection());

	for (auto& image : m_models)
	{
		m_renderShader.SetMat44("model", image.model);

		image.image.Bind();

		glBindVertexArray(m_quadBuffer.m_VAO);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	unsigned size = static_cast<unsigned>(m_models.size());
	m_models.clear();
	m_models.reserve(size);
}
