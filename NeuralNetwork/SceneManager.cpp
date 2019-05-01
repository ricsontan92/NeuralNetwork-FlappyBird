#include "SceneManager.h"

#include "PhysicsManager.h"
#include "TrainingScene.h"
#include "ANNTrainer.h"
#include "GameScene.h"

SceneManager::SceneManager(GraphicsManager& graphicsMgr) :	m_graphicsMgr(graphicsMgr),
															m_hasInit(false),
															m_renderSceneIdx(-1)
{

}

SceneManager::~SceneManager()
{
	for (auto & scene : m_trainingScenes)
		scene->StopScene();
	for (auto & trainer : m_trainers)
		trainer.wait();	
}

void SceneManager::Init(const ScenesConfig& config)
{
	m_config = config;

	// training scenes
	unsigned sceneCount = m_config.m_threaded ? max(1, std::thread::hardware_concurrency() - 1) : 1;
	unsigned perScene	= config.m_samplesCount / sceneCount;
	for (unsigned i = 1; i <= sceneCount; ++i)
	{
		unsigned remainder = i == sceneCount ? (config.m_samplesCount % sceneCount) : 0;
		m_trainingScenes.emplace_back(std::make_shared<TrainingScene>(m_graphicsMgr, perScene + remainder));
		m_trainers.emplace_back(std::async(&SceneManager::SetupTrainingScene, this, std::ref(*m_trainingScenes.back())));
	}

	m_hasInit = true;
}

void SceneManager::Update(float dt)
{
	if (!m_hasInit)
	{
		return;
	}

	if (m_gameScene)
	{
		m_gameScene->Update(dt);
	}
	else
	{
		if(m_renderSceneIdx >= 0 && static_cast<size_t>(m_renderSceneIdx) < m_trainingScenes.size())
			m_trainingScenes[m_renderSceneIdx]->GetPhysicsManager().RenderDebugShapes();

		// check if training is completed
		if (IsAllTrainingCompleted())
		{
			ANNTrainer combinedTrainer;
			for (auto & trainer : m_trainers)
			{
				trainer.wait();
				combinedTrainer += trainer.get();
			}
			m_trainers.clear();
			m_gameScene = std::make_unique<GameScene>(m_graphicsMgr, combinedTrainer);
			m_trainingScenes.clear();
		}
	}
}

const std::vector<std::shared_ptr<TrainingScene>> & SceneManager::GetTrainingScenes() const
{
	return m_trainingScenes;
}

const std::unique_ptr<GameScene> & SceneManager::GetGameScene() const
{
	return m_gameScene;
}

int SceneManager::GetRenderingSceneIdx() const
{
	return m_renderSceneIdx;
}

void SceneManager::SetRenderingSceneIdx(int id)
{
	m_renderSceneIdx = id;
}

void SceneManager::Unload()
{
	m_hasInit = false;
	m_renderSceneIdx = -1;
	m_config = ScenesConfig();
	m_gameScene.reset();
	m_trainers.clear();
	m_trainingScenes.clear();
}

bool SceneManager::IsAllTrainingCompleted() const
{
	for (auto & scene : m_trainingScenes)
	{
		if (scene->IsTraining())
		{
			return false;
		}
	}
	return true;
}

ANNTrainer SceneManager::SetupTrainingScene(TrainingScene& scene)
{
	while (scene.IsRunning() && scene.IsTraining())
	{
		scene.Update(m_config.m_discreteDT);
	}
	return scene.GetANNTrainer();
}