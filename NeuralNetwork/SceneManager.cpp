#include "SceneManager.h"

#include "GraphicsManager.h"
#include "PhysicsManager.h"
#include "TrainingScene.h"

SceneManager::SceneManager(GraphicsManager& graphicsMgr) :	m_graphicsMgr(graphicsMgr),
															m_hasInit(false),
															m_sceneSpd(1)
{

}

SceneManager::~SceneManager()
{
}

void SceneManager::Init(const ScenesConfig& config)
{
	m_config = config;

	// training scenes
	m_trainingScene = std::make_shared<TrainingScene>(m_graphicsMgr, m_config.m_agentCount);

	m_hasInit = true;
}

void SceneManager::Update(float dt)
{
	if (!m_hasInit)
	{
		return;
	}

	if (m_trainingScene)
	{
		for (unsigned i = 0; i < m_sceneSpd; ++i)
		{
			m_trainingScene->Update(dt);
		}

		m_trainingScene->GetPhysicsManager().RenderDebugShapes();
	}
}

void SceneManager::Unload()
{
	m_hasInit = false;
	m_config  = ScenesConfig();
}

unsigned SceneManager::GetSceneSpeed() const
{
	return m_sceneSpd;
}

std::shared_ptr<TrainingScene> const& SceneManager::GetTrainingScene() const
{
	return m_trainingScene;
}

void SceneManager::SetSceneSpeed(unsigned speed)
{
	m_sceneSpd = min(max(speed, 1), 1 << 10);
}
