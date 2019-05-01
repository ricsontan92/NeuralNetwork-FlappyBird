#pragma once

#include "GraphicsBuffers.h"
#include "GLShader.h"

#include <memory>

extern const float	gQuadVertices[20];
extern unsigned int gQuadIndices[6];
extern float		gLineVertices[20];
extern unsigned int gLineIndices[2];

class DebugDrawer;
class GLRenderer;
class GLImage2D;
class Camera;

class GraphicsManager
{
public:
	GraphicsManager();
	void Render() const;
	void StartFrame() const;
	void EndFrame() const;

	// graphics utils
	void EnableAlphaBlend() const;

	// gettors
	DebugDrawer & GetDebugDrawer();
	DebugDrawer const & GetDebugDrawer() const;

	GLRenderer & GetRenderer();
	GLRenderer const & GetRenderer() const;

	Camera & GetMainCamera();
	Camera const & GetMainCamera() const;

	void SetVirtualWinsize(int width, int height);
	void SetBackgroundColor(const math::vec4 & color);
	// settors
	void SetViewport(int minX, int minY, int maxX, int maxY);

private:
	void RenderBackground() const;

	float							m_aspectRatio;
	std::unique_ptr<GLRenderer>		m_renderer;
	std::unique_ptr<DebugDrawer>	m_debugDrawer;
	std::unique_ptr<Camera>			m_mainCam;

	GraphicsBuffers					m_bgBuffer;
	GLShader						m_bgShader;
};