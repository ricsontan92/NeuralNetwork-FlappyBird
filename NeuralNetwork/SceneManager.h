#pragma once

#include <vector>
#include <future>

class ANNTrainer;
class TrainingScene;
class GraphicsManager;

class SceneManager
{
public:
	struct ScenesConfig
	{
		unsigned	m_agentCount	= 50;
		float		m_discreteDT	= 1.f / 60.f;
	};

	SceneManager(GraphicsManager& graphicsMgr);
	~SceneManager();
	void Init(const ScenesConfig& config);
	void Update(float dt);

	void Unload();

	unsigned GetSceneSpeed() const;
	std::shared_ptr<TrainingScene> const& GetTrainingScene() const;

	void SetSceneSpeed(unsigned speed);

private:
	bool										m_hasInit;
	ScenesConfig								m_config;
	GraphicsManager&							m_graphicsMgr;
	std::shared_ptr<TrainingScene>				m_trainingScene;
	unsigned									m_sceneSpd;
};