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

void GLRenderer::AddTextureToScene(const TextureInfo & textureInfo, const math::mat4 & model)
{
	std::unique_ptr<GLImage2D>& pTexture = m_textureCache[textureInfo.m_textureName];

	if (pTexture == nullptr)
	{
		pTexture = std::make_unique<GLImage2D>();
		if (!pTexture->LoadTexture(textureInfo.m_textureName))
		{
			return;
		}
	}

	RenderModel renderModel{ *pTexture ,textureInfo.m_cols, textureInfo.m_rows, textureInfo.m_currFrame, textureInfo.m_tint, model };
	m_models.emplace_back(std::move(renderModel));
}

void GLRenderer::AddTextureToScene(const TextureInfo & textureName, const math::vec3 & position, const math::vec3& scale, float degrees)
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
		float currCol = static_cast<float>(image.m_currFrame % image.m_cols) / static_cast<float>(image.m_cols);
		float currRow = (image.m_currFrame / image.m_cols) / static_cast<float>(image.m_rows);

		m_renderShader.SetMat44("model", image.m_model);
		m_renderShader.SetVec4("textTint", image.m_tint);
		m_renderShader.SetVec2("texcoordoffset", math::vec2(currCol, currRow));
		m_renderShader.SetVec2("textframecount", math::vec2(static_cast<float>(image.m_cols), static_cast<float>(image.m_rows)));

		image.m_image.Bind();

		glBindVertexArray(m_quadBuffer.m_VAO);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	unsigned size = static_cast<unsigned>(m_models.size());
	m_models.clear();
	m_models.reserve(size);
}
