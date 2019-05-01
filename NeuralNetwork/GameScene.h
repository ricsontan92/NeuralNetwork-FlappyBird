#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <future>
#include "FANN/fann.h"

#include "math.h"
#include "JumpData.h"
#include "Randomizer.h"
#include "TrainingScene.h"
#include "SceneConstants.h"

class ANNTrainer;
class ANNWrapper;
class PhysicsBody;
class PhysicsManager;
class GraphicsManager;

struct ContactInfo;
struct JumpData;

class GameScene : private TrainingScene
{
public:
	GameScene(GraphicsManager& graphicsMgr, const ANNTrainer& trainer);
	~GameScene();

	virtual void Update(float dt) override;

	unsigned GetSceneSpeed() const;
	unsigned GetCurrentScore() const;
	unsigned GetMaxScore() const;
	float GetAverageScore() const;
	bool IsTraining() const override;

	const ANNWrapper& GetANN() const;

	const std::map<unsigned, unsigned> & GetScoreStatistics() const;
	void SetSceneSpeed(unsigned set);
	void PrematureEndTraining();

private:
	virtual void UpdateMainEntity(float dt) override;
	virtual void RestartGame() override;
	virtual void ContactEnterCallback(const ContactInfo & contactInfo) override;

	bool MyEpochCallback(unsigned epoch);

	unsigned							m_sceneSpd;

	std::unique_ptr<std::future<void>>	m_annFuture;
	std::unique_ptr<ANNWrapper>			m_ann;

	unsigned							m_maxScore, m_currScore;
	unsigned							m_gameCount, m_accumScore;

	std::map<unsigned, unsigned>		m_scoresStats;
	bool								m_cutTraining;

	GraphicsManager&					m_graphicsMgr;
};