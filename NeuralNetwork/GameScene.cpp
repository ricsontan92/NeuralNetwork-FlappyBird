#include "GameScene.h"

#include "GLRenderer.h"
#include "ANNTrainer.h"
#include "ANNWrapper.h"
#include "PhysicsBody.h"
#include "SceneConstants.h"
#include "PhysicsManager.h"
#include "GraphicsManager.h"
#include "PhysicsContactListener.h"
#include <iostream>

GameScene::GameScene(GraphicsManager& graphicsMgr, const ANNTrainer& trainer) :
	TrainingScene(graphicsMgr, 0),
	m_maxScore(0),
	m_currScore(0),
	m_sceneSpd(1),
	m_gameCount(0),
	m_accumScore(0),
	m_cutTraining(false),
	m_graphicsMgr(graphicsMgr)
{
	ANNWrapper::ANNConfig annConfig;
	annConfig.m_numInputs			= 2;
	annConfig.m_numOutputs			= 1;
	annConfig.m_numLayers			= 4;
	annConfig.m_numNeuronsInHidden	= 3;
	annConfig.m_maxErr				= 0.001f;
	annConfig.m_maxEpochs			= 100000;
	annConfig.m_epochsBtwnReports	= 5000;

	m_ann = std::make_unique<ANNWrapper>(annConfig);
	m_ann->SetTrainer(std::make_unique<ANNTrainer>(trainer));

	m_annFuture = std::make_unique<std::future<void> >
	(
		std::async([this]() {
			m_ann->Train();
		})
	);

	m_ann->SetEpochCallback(&GameScene::MyEpochCallback, this);
}

GameScene::~GameScene()
{
	StopScene();
	if (m_annFuture && m_annFuture->valid()) // if still not yet done training
		m_annFuture->wait();
}

void GameScene::Update(float dt)
{
	if (m_annFuture)
	{
		if (m_annFuture->wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			m_annFuture.reset();
		}
		else
		{
			return;
		}
	}

	for (unsigned i = 0; i < m_sceneSpd; ++i)
	{
		if (m_gameRestarting)
		{
			EndGame();
			StartGame();
		}
		else
		{
			UpdateMainEntity(dt);
			UpdateBulletEntity(dt);
			m_physicsMgr->Update(dt);
		}
	}

	m_physicsMgr->RenderDebugShapes();
}

unsigned GameScene::GetSceneSpeed() const
{
	return m_sceneSpd;
}

unsigned GameScene::GetCurrentScore() const
{
	return m_currScore;
}

float GameScene::GetAverageScore() const
{
	if (m_gameCount == 0)
		return 0.f;
	return m_accumScore / static_cast<float>(m_gameCount);
}

bool GameScene::IsTraining() const
{
	return !m_cutTraining && m_annFuture;
}

const ANNWrapper& GameScene::GetANN() const
{
	return *m_ann;
}

unsigned GameScene::GetMaxScore() const
{
	return m_maxScore;
}

const std::map<unsigned, unsigned> & GameScene::GetScoreStatistics() const
{
	return m_scoresStats;
}

void GameScene::SetSceneSpeed(unsigned set)
{
	m_sceneSpd = min(max(1, set), 1 << 12);
}

void GameScene::PrematureEndTraining()
{
	m_cutTraining = true;
}

void GameScene::UpdateMainEntity(float dt)
{
	if (!m_isPlayerOnGround)
	{
		return;
	}

	if (m_bulletEntity)
	{
		float inputs[(unsigned)JumpData::INPUT_TYPE::COUNT];
		inputs[(unsigned)JumpData::INPUT_TYPE::X_DIST]	= m_mainEntity->GetPosition().x - m_bulletEntity->GetPosition().x;
		inputs[(unsigned)JumpData::INPUT_TYPE::X_SPEED] = m_bulletEntity->GetVelocity().x;
		std::vector<fann_type> outputs = m_ann->Run(inputs);

		if (outputs[(unsigned)JumpData::OUTPUT_TYPE::JUMP] >= 0.9f)
		{
			m_mainEntity->AddForceToCenter(math::vec2(0.f, PLAYER_JUMP_FORCE));
			m_isPlayerOnGround = false;
		}
	}
}

void GameScene::ContactEnterCallback(const ContactInfo & contactInfo)
{
	bool AisBullet			= "Bullet"			== contactInfo.m_bodyA->GetName();
	bool BisBullet			= "Bullet"			== contactInfo.m_bodyB->GetName();
	bool AisBulletDestroy	= "BulletDestroy"	== contactInfo.m_bodyA->GetName();
	bool BisBulletDestroy	= "BulletDestroy"	== contactInfo.m_bodyB->GetName();
	bool AisMainPlayer		= "MainPlayer"		== contactInfo.m_bodyA->GetName();
	bool BisMainPlayer		= "MainPlayer"		== contactInfo.m_bodyB->GetName();
	bool AisGround			= "Ground"			== contactInfo.m_bodyA->GetName();
	bool BisGround			= "Ground"			== contactInfo.m_bodyB->GetName();

	if (AisBullet && BisBulletDestroy)
	{
		if (!contactInfo.m_bodyA->IsDestroyed())
		{
			contactInfo.m_bodyA->Destroy();
			m_bulletEntity.reset();
			m_currScore++;
		}
	}
	else if (BisBullet && AisBulletDestroy) 
	{ 
		if (!contactInfo.m_bodyB->IsDestroyed())
		{
			contactInfo.m_bodyB->Destroy();
			m_bulletEntity.reset();
			m_currScore++;
		}
	}
	else if ((AisBullet && BisMainPlayer) || (BisBullet && AisMainPlayer))
	{
		// failed
		RestartGame();
	}
	else if ((AisGround && BisMainPlayer) || (BisGround && AisMainPlayer))
	{
		m_isPlayerOnGround = true;
	}
}

bool GameScene::MyEpochCallback(unsigned epoch)
{
	return IsRunning() && !m_cutTraining;
}

void GameScene::RestartGame()
{
	m_gameRestarting = true;

	m_maxScore = max(m_maxScore, m_currScore);

	m_scoresStats[m_currScore]++;

	m_accumScore += m_currScore;
	m_gameCount++;

	m_currScore = 0;
}
