#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>

#include "FANN/fann.h"

#include "math.h"
#include "Randomizer.h"

class ANNWrapper;
class PhysicsBody;
class PhysicsManager;
class GraphicsManager;

struct ContactInfo;

class TrainingScene
{
public:
	TrainingScene(GraphicsManager& graphicsMgr, unsigned agentCount);
	virtual ~TrainingScene();
	virtual void Update(float dt);

	PhysicsManager & GetPhysicsManager() const;
	unsigned GetCurrentScore() const;
	unsigned GetMaxScore() const;
	unsigned GetCurrentGeneration() const;
	unsigned GetLiveBirdCount() const;

protected:
	void StartGame();
	void RestartGame();

	virtual void ContactEnterCallback(const ContactInfo & contactInfo);
	virtual void ContactExitCallback(const ContactInfo & contactInfo);

	bool							m_gameRestarting;
	std::unique_ptr<PhysicsManager> m_physicsMgr;

private:
	enum class ObjectType
	{
		BIRD		= 1 << 1,
		GROUND		= 1 << 2,
		OBSTACLE	= 1 << 3,
		DESTROYER	= 1 << 4
	};

	enum class InputType
	{
		DIST_FROM_OBSTACLE,
		HEIGHT_FROM_NEAREST_HOLE,
		HEIGHT_FROM_SECOND_NEAREST_HOLE,
		VERTICAL_SPEED,
		COUNT
	};

	struct BirdInfo
	{
		std::unique_ptr<ANNWrapper>		m_ann;
		std::shared_ptr<PhysicsBody>	m_bird;
		float							m_delay;
		float							m_animTimer;
		unsigned						m_currFrame;
		math::vec4						m_birdColor;
	};

	struct WeightInfo
	{
		std::vector<fann_type>	m_weights;
		float					m_distFromHole;
		unsigned				m_currPointsOnDeath;
	};

	std::shared_ptr<PhysicsBody> GetNearestObstacle() const;
	std::shared_ptr<PhysicsBody> GetSecondNearestObstacle() const;
	BirdInfo SpawnBird() const;
	BirdInfo SpawnBird(const std::vector<fann_type>& weights ) const;
	void SpawnObstacle();

	// genetic algorithm
	void Selection();
	void Crossover();

	float					m_obstacleSpawnTimer;
	std::vector<BirdInfo>	m_birds;
	unsigned				m_agentCount;
	Randomizer				m_randomizer;
	std::vector<WeightInfo>	m_collectedWeights;
	GraphicsManager&		m_graphicsMgr;
	unsigned				m_currScore, m_maxScore;
	unsigned				m_currGeneration;
};