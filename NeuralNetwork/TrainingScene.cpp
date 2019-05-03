#include "TrainingScene.h"

#include "ANNWrapper.h"
#include "GLRenderer.h"
#include "DebugDrawer.h"
#include "PhysicsBody.h"
#include "PhysicsManager.h"
#include "SceneConstants.h"
#include "GraphicsManager.h"
#include "PhysicsContactListener.h"

#include <ctime>
#include <iostream>

TrainingScene::TrainingScene(GraphicsManager& graphicsMgr, unsigned agentCount) :
	m_physicsMgr(std::make_unique<PhysicsManager>(math::vec2(0.f, -9.8f), graphicsMgr.GetDebugDrawer())),
	m_gameRestarting(false),
	m_agentCount(agentCount),
	m_randomizer(-1.f, 1.f),
	m_obstacleSpawnTimer(0.f),
	m_graphicsMgr(graphicsMgr),
	m_currScore(0),
	m_maxScore(0),
	m_currGeneration(0),
	m_bgTimer(0.f)
{
	m_physicsMgr->GetContactListener().SetBeginContactCallbackFunction(&TrainingScene::ContactEnterCallback, this);
	m_physicsMgr->GetContactListener().SetEndContactCallbackFunction(&TrainingScene::ContactExitCallback, this);
}

TrainingScene::~TrainingScene()
{
	m_physicsMgr->GetContactListener().ClearListenerFunctions();
}

void TrainingScene::Update(float dt)
{
	// update bg parallax bg 
	m_bgTimer += dt * 0.25f;
	
	m_physicsMgr->Update(dt);

	if ((m_obstacleSpawnTimer -= dt) <= 0.f)
	{
		SpawnObstacle();
		m_obstacleSpawnTimer = SceneConstants::ObstacleSpawnTime;
	}

	for (auto & bird : m_birds)
	{
		PhysicBodyPtr nearestObj = GetNearestObstacle();
		PhysicBodyPtr secNearestObj = GetSecondNearestObstacle();
		float computeMid = 0.5f * (reinterpret_cast<PhysicsBody*>(nearestObj->GetUserData())->GetPosition().y + nearestObj->GetPosition().y);
		float computeMid2 = secNearestObj ? 0.5f * (reinterpret_cast<PhysicsBody*>(secNearestObj->GetUserData())->GetPosition().y + secNearestObj->GetPosition().y) : 0.f;
		if ((bird.m_delay -= dt) <= 0.f)
		{
			float input[static_cast<unsigned>(InputType::COUNT)];
			input[static_cast<unsigned>(InputType::DIST_FROM_OBSTACLE)]					= nearestObj->GetPosition().x - bird.m_bird->GetPosition().x;
			input[static_cast<unsigned>(InputType::HEIGHT_FROM_NEAREST_HOLE)]			= computeMid - bird.m_bird->GetPosition().y;
			input[static_cast<unsigned>(InputType::HEIGHT_FROM_SECOND_NEAREST_HOLE)]	= computeMid2 - bird.m_bird->GetPosition().y;
			input[static_cast<unsigned>(InputType::VERTICAL_SPEED)]						= bird.m_bird->GetVelocity().y;

			if (bird.m_ann->Run(input)[0] >= 0.f)
			{
				bird.m_bird->AddForceToCenter(math::vec2(0.f, SceneConstants::FlapStrength));
				bird.m_delay = SceneConstants::FlapDelay;
			}
		}

		if ((bird.m_animTimer += dt) >= 0.025f)
		{
			bird.m_animTimer = 0.f;
			bird.m_currFrame = (bird.m_currFrame + 1) % 14;
		}
	}

	if (m_birds.empty())
	{
		RestartGame();
	}
}

void TrainingScene::Render() const
{
	// render background
	{
		GLRenderer::TextureInfo textureInfo;
		textureInfo.m_textureName = "Assets/background.png";
		textureInfo.m_cols = 1;
		textureInfo.m_rows = 1;
		textureInfo.m_currFrame = m_bgTimer;
		textureInfo.m_tint = math::vec4(1.f, 1.f, 1.f, 1.f);
		m_graphicsMgr.GetRenderer().AddTextureToScene(textureInfo, math::vec2(), m_graphicsMgr.GetVirtualWindowSize(), 0.f);
	}

	// render birds
	for (auto & bird : m_birds)
	{
		GLRenderer::TextureInfo textureInfo;
		textureInfo.m_textureName = "Assets/bird_anim.png";
		textureInfo.m_cols = 5;
		textureInfo.m_rows = 3;
		textureInfo.m_currFrame = static_cast<float>(bird.m_currFrame);
		textureInfo.m_tint = bird.m_birdColor;
		m_graphicsMgr.GetRenderer().AddTextureToScene(textureInfo, bird.m_bird->GetPosition(), bird.m_bird->GetSize(), 0.f);
	}

	// render obstacles
	for (auto & obstacle : m_obstacles)
	{
		GLRenderer::TextureInfo textureInfo;
		textureInfo.m_textureName = "Assets/pole.png";
		textureInfo.m_cols = 1;
		textureInfo.m_rows = 1;
		textureInfo.m_currFrame = 0;
		textureInfo.m_tint = math::vec4(1.f, 1.f, 1.f, 1.f);

		m_graphicsMgr.GetRenderer().AddTextureToScene(textureInfo, obstacle->GetPosition(), obstacle->GetSize(), obstacle->GetAngle());
	}
}

PhysicsManager & TrainingScene::GetPhysicsManager() const
{
	return *m_physicsMgr;
}

unsigned TrainingScene::GetCurrentScore() const
{
	return m_currScore >> 1;
}

unsigned TrainingScene::GetMaxScore() const
{
	return m_maxScore >> 1;
}

unsigned TrainingScene::GetCurrentGeneration() const
{
	return m_currGeneration;
}

unsigned TrainingScene::GetLiveBirdCount() const
{
	return m_birds.size();
}

void TrainingScene::StartGame()
{
	float height = 385.f;

	PhysicBodyPtr groundA = m_physicsMgr->AddBox(math::vec2(0.f, -height), math::vec2(1500.f, 50.f), 0.f, PhysicsManager::BodyType::STATIC);
	groundA->SetCategoryBits(static_cast<uint16>(ObjectType::GROUND));
	groundA->SetName("Ground");
	groundA->SetDebugFill(true);
	groundA->SetDebugColor(DEBUG_YELLOW);

	PhysicBodyPtr groundB = m_physicsMgr->AddBox(math::vec2(0.f, height), math::vec2(1500.f, 50.f), 0.f, PhysicsManager::BodyType::STATIC);
	groundB->SetCategoryBits(static_cast<uint16>(ObjectType::GROUND));
	groundB->SetName("Ground");
	groundB->SetDebugFill(true);
	groundB->SetDebugColor(DEBUG_YELLOW);

	PhysicBodyPtr destroyer = m_physicsMgr->AddBox(math::vec2(-750.f, 0.f), math::vec2(50.f, 1000.f), 0.f, PhysicsManager::BodyType::STATIC);
	destroyer->SetCategoryBits(static_cast<uint16>(ObjectType::DESTROYER));
	destroyer->SetName("Destroyer");

	m_currGeneration++;
}

void TrainingScene::RestartGame()
{
	// reset variables
	m_currScore				= 0;
	m_obstacleSpawnTimer	= 0.f;
	m_bgTimer				= 0.f;

	// reset physics
	m_obstacles.clear();
	m_physicsMgr->Clear();
	
	// start game
	StartGame();

	// start of the scene, randomize weights first
	if (m_collectedWeights.empty())
	{
		for (unsigned i = 0; i < m_agentCount; ++i)
		{
			m_birds.emplace_back(std::move(SpawnBird()));
		}
	}
	else
	{
		Selection();
		Crossover();
	}
}

void TrainingScene::ContactEnterCallback(const ContactInfo & contactInfo)
{
	bool isABird		= contactInfo.m_bodyA->GetName() == "Bird";
	bool isBBird		= contactInfo.m_bodyB->GetName() == "Bird";
	bool isAObstacle	= contactInfo.m_bodyA->GetName() == "Obstacle";
	bool isBObstacle	= contactInfo.m_bodyB->GetName() == "Obstacle";
	bool isADestroyer	= contactInfo.m_bodyA->GetName() == "Destroyer";
	bool isBDestroyer	= contactInfo.m_bodyB->GetName() == "Destroyer";
	bool isAGround		= contactInfo.m_bodyA->GetName() == "Ground";
	bool isBGround		= contactInfo.m_bodyB->GetName() == "Ground";

	if ((isAObstacle && isBDestroyer) || (isBObstacle && isADestroyer))
	{
		PhysicsBody* body = isAObstacle ? contactInfo.m_bodyA : contactInfo.m_bodyB;
		auto it = std::find_if(m_obstacles.begin(), m_obstacles.end(), [&](const PhysicBodyPtr& ptr) { return &(*ptr) == body; });
		if (it != m_obstacles.end())
		{
			body->Destroy();
			++m_currScore;
			m_maxScore = max(m_maxScore, m_currScore);
			m_obstacles.erase(it);
		}
	}
	else if (	(isABird && isBObstacle) || 
				(isBBird && isAObstacle) ||
				(isABird && isBGround)	 ||
				(isBBird && isAGround)	)
	{
		PhysicBodyPtr nearestObj = GetNearestObstacle();
		PhysicsBody* pBody = isABird ? contactInfo.m_bodyA : contactInfo.m_bodyB;
		auto it = std::find_if(m_birds.begin(), m_birds.end(), [&](const BirdInfo& info) { return &(*info.m_bird) == pBody; });
		if (it != m_birds.end())
		{
			WeightInfo weight;
			weight.m_distFromHole		= fabs(pBody->GetPosition().y - 0.5f * (reinterpret_cast<PhysicsBody*>(nearestObj->GetUserData())->GetPosition().y + nearestObj->GetPosition().y));
			weight.m_currPointsOnDeath	= m_currScore;
			weight.m_weights			= it->m_ann->GetWeights();
			m_collectedWeights.emplace_back(std::move(weight));

			it->m_bird->Destroy();
			m_birds.erase(it);
		}
	}
}

void TrainingScene::ContactExitCallback(const ContactInfo & contactInfo)
{
}

std::shared_ptr<PhysicsBody> TrainingScene::GetNearestObstacle() const
{
	float birdBackX = m_birds.front().m_bird->GetPosition().x - m_birds.front().m_bird->GetSize().x * 0.5f;
	std::vector<PhysicBodyPtr> const & allBodies = m_physicsMgr->GetAllBodies();

	int idx = -1;
	float currDist = FLT_MAX;

	for (unsigned i = 0; i < allBodies.size(); ++i)
	{
		if (allBodies[i]->GetName() == "Obstacle")
		{
			float xFront = allBodies[i]->GetPosition().x + allBodies[i]->GetSize().x * 0.5f;
			float diff = xFront - birdBackX;
			if (diff > 0.f && currDist > diff)
			{
				currDist = diff;
				idx = static_cast<int>(i);
			}
		}
	}
	return idx == -1 ? nullptr : allBodies[idx];
}

std::shared_ptr<PhysicsBody> TrainingScene::GetSecondNearestObstacle() const
{
	float birdBackX = m_birds.front().m_bird->GetPosition().x - m_birds.front().m_bird->GetSize().x * 0.5f;
	std::vector<PhysicBodyPtr> const & allBodies = m_physicsMgr->GetAllBodies();

	int idx = -1;
	float currDist = FLT_MAX, currDist2 = FLT_MAX;

	for (unsigned i = 0; i < allBodies.size(); ++i)
	{
		if (allBodies[i]->GetName() == "Obstacle")
		{
			float xFront = allBodies[i]->GetPosition().x + allBodies[i]->GetSize().x * 0.5f;
			float diff = xFront - birdBackX;
			if (diff > 0.f)
			{
				if (currDist > diff)
				{
					currDist = diff;
				}
				else if (currDist < diff && diff < currDist2)
				{
					currDist2 = diff;
					idx = static_cast<int>(i);
				}
			}
		}
	}
	return idx == -1 ? nullptr : allBodies[idx];
}

TrainingScene::BirdInfo TrainingScene::SpawnBird() const
{
	BirdInfo info = SpawnBird(std::vector<fann_type>());
	return info;
}

TrainingScene::BirdInfo TrainingScene::SpawnBird(const std::vector<fann_type>& weights) const
{
	BirdInfo info;

	info.m_bird = m_physicsMgr->AddCircle(math::vec2(-400.f, 0.f), SceneConstants::BirdSize * 0.5f, 0.f, PhysicsManager::BodyType::DYNAMIC);

	info.m_bird->SetName("Bird");
	info.m_bird->SetIsSensor(true);
	info.m_bird->SetCategoryBits(static_cast<uint16>(ObjectType::BIRD));
	info.m_bird->SetMaskBits(static_cast<uint16>(ObjectType::GROUND) | static_cast<uint16>(ObjectType::OBSTACLE));
	info.m_bird->SetDensity(8.f);
	info.m_bird->SetGravityScale(3.f);

	info.m_currFrame = 0;
	info.m_animTimer = 0.f;

	static Randomizer colRandomizer(0.f, 1.f);
	info.m_birdColor = math::vec4(colRandomizer.GetRandomFloat(), colRandomizer.GetRandomFloat(), colRandomizer.GetRandomFloat(), 1.f);

	info.m_delay = SceneConstants::FlapDelay;

	ANNWrapper::ANNConfig config;
	config.m_epochsBtwnReports	= 5000;
	config.m_maxEpochs			= 10000;
	config.m_maxErr				= 0.001f;
	config.m_numInputs			= 4;
	config.m_numLayers			= 3;
	config.m_numNeuronsInHidden = 8;
	config.m_numOutputs			= 1;
	info.m_ann = std::make_unique<ANNWrapper>(config);

	if (weights.empty())
		info.m_ann->RandomizeWeights();
	else
		info.m_ann->SetWeights(weights);

	return info;
}

void TrainingScene::SpawnObstacle()
{
	const float obstacleLt = 600.f;
	const float obstacleSPos = 650.f;

	float rndHeight = SceneConstants::HoleDistanceRange * m_randomizer.GetRandomFloat();
	float obstacleHalfHt = (obstacleLt + SceneConstants::HoleHeight) * 0.5f;

	PhysicBodyPtr obstacles[]
	{
		m_physicsMgr->AddBox(math::vec2(obstacleSPos, rndHeight - obstacleHalfHt), math::vec2(90.f, obstacleLt), 0.f, PhysicsManager::BodyType::DYNAMIC),	// lower
		m_physicsMgr->AddBox(math::vec2(obstacleSPos, rndHeight + obstacleHalfHt), math::vec2(90.f, obstacleLt), 180.f, PhysicsManager::BodyType::DYNAMIC)	// upper
	};

	// set user data
	obstacles[0]->SetUserData(&(*obstacles[1]));
	obstacles[1]->SetUserData(&(*obstacles[0]));

	for (auto & obstacle : obstacles)
	{
		obstacle->SetName("Obstacle");
		obstacle->SetCategoryBits(static_cast<uint16>(ObjectType::OBSTACLE));
		obstacle->SetMaskBits(static_cast<uint16>(ObjectType::BIRD) | static_cast<uint16>(ObjectType::DESTROYER));
		obstacle->SetVelocity(math::vec2(-SceneConstants::ObstacleInitialSpeed, 0.f));
		obstacle->SetIsSensor(true);
		obstacle->SetGravityScale(0.f);

		m_obstacles.emplace_back(std::move(obstacle));
	}
}

void TrainingScene::Selection()
{
	// sort points then dist
	std::sort(m_collectedWeights.begin(), m_collectedWeights.end(), [](const WeightInfo& l, const WeightInfo & r)
	{
		if (l.m_currPointsOnDeath > r.m_currPointsOnDeath)
			return true;
		if (l.m_currPointsOnDeath < r.m_currPointsOnDeath)
			return false;
		return l.m_distFromHole < r.m_distFromHole;
	});

	// get only 10%
	int max_parents_count = max(2, static_cast<int>(ceil(m_agentCount * 0.1f)));
	m_collectedWeights = std::vector<WeightInfo>(m_collectedWeights.begin(), m_collectedWeights.begin() + max_parents_count);
}

void TrainingScene::Crossover()
{
	// genetic algorithm starts here
	for (unsigned i = 0; i < m_agentCount; ++i)
	{
		int currIdx = i % m_collectedWeights.size(), other;
		const std::vector<fann_type>& parentA = m_collectedWeights[currIdx].m_weights;

		do
		{
			other = rand() % m_collectedWeights.size();
		} while (other == currIdx);

		const std::vector<fann_type>& parentB = m_collectedWeights[other].m_weights;
		std::vector<fann_type> child = parentA;

		unsigned weightSize = child.size() >> 2;	// 1/4 of the weights will be crossed over 
		for (unsigned i = 0; i < weightSize; ++i)
		{
			float probability = (m_randomizer.GetRandomFloat() + 1.f) * 0.5f;
			if (probability < 0.45f)
			{
				int rndNum = rand() % parentB.size();
				child[rndNum] = parentB[rndNum];									// get gene from B parent
			}
			else if (probability > 0.8f)
			{
				child[rand() % child.size()] = m_randomizer.GetRandomFloat();		// mutated gene
			}
		}

		m_birds.emplace_back(std::move(SpawnBird(child)));
	}

	m_collectedWeights.clear();
}