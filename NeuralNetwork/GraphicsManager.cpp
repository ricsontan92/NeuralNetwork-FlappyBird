#include "GraphicsManager.h"

#include "DebugDrawer.h"
#include "GLRenderer.h"
#include "Camera.h"

GraphicsManager::GraphicsManager() :
	m_mainCam(std::make_unique<Camera>()),
	m_debugDrawer(std::make_unique<DebugDrawer>(*this)),
	m_renderer(std::make_unique<GLRenderer>(*this))
{
	// set virtual stuff
	SetVirtualWinsize(1280, 720);

	// set up background buffer
	m_bgBuffer.SetupVAOs(gLineVertices, gLineIndices);
	m_bgShader.AddShader("Assets/DebugShader.frag", GL_FRAGMENT_SHADER);
	m_bgShader.AddShader("Assets/DebugShader.vert", GL_VERTEX_SHADER);
	m_bgShader.GenShaderProgram();

	// init background color
	SetBackgroundColor(math::vec4(0.f, 0.f, 0.f, 1.f));
}

void GraphicsManager::Render() const
{
	m_renderer->RenderTextures();
	m_debugDrawer->RenderDebugShapes();
}

void GraphicsManager::StartFrame() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear framebuffer (color and depth)
}

void GraphicsManager::EndFrame() const
{
	// clean up
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void GraphicsManager::EnableAlphaBlend() const
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

DebugDrawer & GraphicsManager::GetDebugDrawer()
{
	return *m_debugDrawer;
}

DebugDrawer const & GraphicsManager::GetDebugDrawer() const
{
	return *m_debugDrawer;
}

GLRenderer & GraphicsManager::GetRenderer()
{
	return *m_renderer;
}

GLRenderer const & GraphicsManager::GetRenderer() const
{
	return *m_renderer;
}

Camera & GraphicsManager::GetMainCamera()
{
	return *m_mainCam;
}

Camera const & GraphicsManager::GetMainCamera() const
{
	return *m_mainCam;
}

void GraphicsManager::SetVirtualWinsize(int width, int height)
{
	float half_width = 0.5f * width;
	float half_height = 0.5f * height;
	m_mainCam->SetOrtho(half_width, -half_width, -half_height, half_height, -1.f, 1.f);
	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	m_bgShader.UseProgram();
	const math::mat4 bgScale = math::mat4::Scale(math::vec3(static_cast<float>(width), static_cast<float>(height), 0.f));
	const math::mat4 bgTrans = math::mat4::Translate(math::vec3(0.f, 0.f, 1.f));
	m_bgShader.SetMat44("model", bgTrans * bgScale);
}

void GraphicsManager::SetBackgroundColor(const math::vec4 & color)
{
	m_bgShader.UseProgram();
	m_bgShader.SetVec4("debugColor", color);
}

void GraphicsManager::SetViewport(int minX, int minY, int maxX, int maxY)
{
	int screen_width	= maxX - minX;
	int screen_height	= maxY - minY;

	// figure out the largest area that fits in this resolution at the desired aspect ratio
	int width	= screen_width;
	int height	= (int)(width / m_aspectRatio + 0.5f);

	if (height > screen_height)
	{
		//It doesn't fit our height, we must switch to pillarbox then
		height = screen_height;
		width = (int)(height * m_aspectRatio + 0.5f);
	}

	// set up the new viewport centered in the backbuffer
	int vp_x = (screen_width >> 1) - (width >> 1);
	int vp_y = (screen_height >> 1) - (height >> 1);

	glViewport(vp_x, vp_y, width, height);

	RenderBackground();
}

void GraphicsManager::RenderBackground() const
{
	// render background
	m_bgShader.UseProgram();
	m_bgShader.SetMat44("projView", GetMainCamera().Get2DProjection());
	glBindVertexArray(m_bgBuffer.m_VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

const float gQuadVertices[20] =
{
	// positions			// texture coords
	// positions			// texture Coords
	-0.5f,  -0.5f,	0.0f,	0.0f,	1.0f,	// 0
	-0.5f,	 0.5f,	0.0f,	0.0f,	0.0f,	// 1
	 0.5f,	-0.5f,	0.0f,	1.0f,	1.0f,	// 2
	 0.5f,	 0.5f,	0.0f,	1.0f,	0.0f	// 3
};

unsigned int gQuadIndices[6] = // note that we start from 0!
{
	1, 0, 3,  // first Triangle
	1, 2, 3   // second Triangle
};

float gLineVertices[20] =
{
	 -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
	  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
	  0.5f, 0.5f,  0.0f, 1.0f, 0.0f,
	 -0.5f, 0.5f,  0.0f, 0.0f, 0.0f
};

unsigned int gLineIndices[2] =  // note that we start from 0!
{
	0, 1
};
