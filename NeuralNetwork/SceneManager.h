#pragma once

#include <vector>
#include <future>

class GameScene;
class ANNTrainer;
class TrainingScene;
class GraphicsManager;

class SceneManager
{
public:
	struct ScenesConfig
	{
		unsigned	m_samplesCount	= 50000;
		float		m_discreteDT	= 1.f / 60.f;
		bool		m_threaded		= false;
	};

	SceneManager(GraphicsManager& graphicsMgr);
	~SceneManager();
	void Init(const ScenesConfig& config);
	void Update(float dt);

	const std::vector<std::shared_ptr<TrainingScene>> & GetTrainingScenes() const;
	const std::unique_ptr<GameScene> & GetGameScene() const;
	int GetRenderingSceneIdx() const;

	void SetRenderingSceneIdx(int id);

	void Unload();

private:
	bool IsAllTrainingCompleted() const;
	ANNTrainer SetupTrainingScene(TrainingScene& scene);

	bool										m_hasInit;
	int											m_renderSceneIdx;
	ScenesConfig								m_config;
	GraphicsManager&							m_graphicsMgr;
	std::unique_ptr<GameScene>					m_gameScene;
	std::vector<std::future<ANNTrainer>>		m_trainers;
	std::vector<std::shared_ptr<TrainingScene>> m_trainingScenes;
};