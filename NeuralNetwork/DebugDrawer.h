#pragma once

#include <vector>

#include "GraphicsBuffers.h"
#include "GLShader.h"
#include "math.h"

static const math::vec4 DEBUG_RED		= math::vec4(1.00f, 0.41f, 0.38f, 1.0f);
static const math::vec4 DEBUG_GREEN		= math::vec4(0.47f, 0.87f, 0.47f, 1.0f);
static const math::vec4 DEBUG_BLUE		= math::vec4(0.68f, 0.78f, 0.90f, 1.0f);
static const math::vec4 DEBUG_YELLOW	= math::vec4(0.99f, 0.99f, 0.59f, 1.0f);

class GraphicsManager;
class DebugDrawer
{
	struct DebugModel
	{
		GLuint vao;
		math::mat4 model;
		math::vec4 color;

		GLenum primitive;
		GLsizei count;
	};

public:
	DebugDrawer(GraphicsManager& graphicsMgr);
	~DebugDrawer();

	void RenderDebugShapes();

	// utils
	void AddDebugCircle(const math::mat4& model, const math::vec4& color);
	void AddDebugBox(const math::mat4& model, const math::vec4& color);
	void AddDebugLine(const math::vec3& start, const math::vec3& end, const math::vec4& color, float thickness = 5.f);

	void AddFilledDebugBox(const math::mat4& model, const math::vec4& color);
	void AddDebugFilledCircle(const math::vec3& worldpos, float radius, const math::vec4& color);
	void AddDebugFilledCircle(const math::mat4& model, const math::vec4& color);

private:
	GLShader m_debugShader;
	std::vector<DebugModel> m_models;
	GraphicsManager& m_graphicsMgr;

	GraphicsBuffers m_boxBuffer, m_lineBuffer, m_circleBuffer;

	void InitializeBuffers();
};