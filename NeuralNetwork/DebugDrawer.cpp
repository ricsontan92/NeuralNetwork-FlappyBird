#include <GL/glew.h>

#include "Camera.h"
#include "DebugDrawer.h"
#include "GraphicsManager.h"

#define CIRCLE_SEGMENTS 32

void DebugDrawer::InitializeBuffers()
{
	m_boxBuffer.SetupVAOs(gLineVertices, gLineIndices);
	m_lineBuffer.SetupVAOs(gQuadVertices, gQuadIndices);

	//Vertex Buffers ( circle )
	{
		std::vector<float>		circleVerts;
		std::vector<unsigned>	circleIndices;
		for (unsigned i = 0; i < CIRCLE_SEGMENTS; ++i)
		{
			circleVerts.push_back(cosf(2.0f * PIf * i / static_cast<float>(CIRCLE_SEGMENTS)));
			circleVerts.push_back(sinf(2.0f * PIf * i / static_cast<float>(CIRCLE_SEGMENTS)));
			circleVerts.push_back(0.f);
			circleVerts.push_back(0.f);
			circleVerts.push_back(0.f);

			circleIndices.push_back(i);
			circleIndices.push_back((i + 1) % CIRCLE_SEGMENTS);
		}

		m_circleBuffer.SetupVAOs(circleVerts, circleIndices);
	}

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	// set up all enable stuff
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

DebugDrawer::DebugDrawer(GraphicsManager& graphicsMgr) : 
	m_graphicsMgr(graphicsMgr)
{
	m_debugShader.AddShader("Assets/DebugShader.frag", GL_FRAGMENT_SHADER);
	m_debugShader.AddShader("Assets/DebugShader.vert", GL_VERTEX_SHADER);
	m_debugShader.GenShaderProgram();

	InitializeBuffers();
}

void DebugDrawer::AddDebugCircle(const math::mat4& model, const math::vec4& color)
{
	DebugModel debugModel{ m_circleBuffer.m_VAO , model , color, GL_LINE_LOOP, 32 };
	m_models.push_back(debugModel);
}

void DebugDrawer::AddDebugBox(const math::mat4& model, const math::vec4& color)
{
	DebugModel debugModel{ m_boxBuffer.m_VAO , model , color, GL_LINE_LOOP, 4 };
	m_models.push_back(debugModel);
}

void DebugDrawer::AddDebugLine(const math::vec3& start, const math::vec3& end, const math::vec4& color, float thickness)
{
	math::vec2 diff = end.xy() - start.xy();

	math::mat4 translate	= math::mat4::Translate((start + end) * 0.5f);
	math::mat4 rotate		= math::mat4::Rotate2D(RAD_TO_DEG(atan2f(diff.y, diff.x)));
	math::mat4 scale		= math::mat4::Scale(math::vec3(diff.Len(), thickness, 0.f));

	DebugModel debugModel{ m_lineBuffer.m_VAO , translate * rotate * scale, color, GL_TRIANGLE_STRIP, 4 };
	m_models.push_back(debugModel);
}

void DebugDrawer::AddFilledDebugBox(const math::mat4& model, const math::vec4& color)
{
	DebugModel debugModel{ m_boxBuffer.m_VAO , model , color, GL_TRIANGLE_FAN, 4 };
	m_models.push_back(debugModel);
}

void DebugDrawer::AddDebugFilledCircle(const math::vec3& worldpos, float radius, const math::vec4& color)
{
	math::mat4 translate = math::mat4::Translate(worldpos);
	math::mat4 scale = math::mat4::Scale(radius);
	AddDebugFilledCircle(translate * scale, color);
}

void DebugDrawer::AddDebugFilledCircle(const math::mat4& model, const math::vec4& color)
{
	DebugModel debugModel{ m_circleBuffer.m_VAO , model, color, GL_TRIANGLE_FAN, 32 };
	m_models.push_back(debugModel);
}

DebugDrawer::~DebugDrawer()
{
}

void DebugDrawer::RenderDebugShapes()
{
	m_graphicsMgr.EnableAlphaBlend();
		
	// render...
	m_debugShader.UseProgram();
	m_debugShader.SetMat44("projView", m_graphicsMgr.GetMainCamera().Get2DProjection());
	
	for (auto& debugShape : m_models)
	{
		m_debugShader.SetVec4("debugColor", debugShape.color);
		m_debugShader.SetMat44("model",		debugShape.model);
	
		glBindVertexArray(debugShape.vao);
	
		glDrawArrays(debugShape.primitive, 0, debugShape.count);
	}
	
	unsigned size = static_cast<unsigned>(m_models.size());
	m_models.clear();
	m_models.reserve(size);
}