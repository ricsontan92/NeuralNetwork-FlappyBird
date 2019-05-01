#pragma once

#include <string>
#include <vector>
#include <memory>

#include "FANN/fann.h"

#include "math.h"
#include "Randomizer.h"

class ANNTrainer;
class ANNWrapper;
class PhysicsBody;
class PhysicsManager;
class GraphicsManager;

struct ContactInfo;
struct JumpData;

class TrainingScene
{
public:
	TrainingScene(GraphicsManager& graphicsMgr, unsigned maxTrainingCount);
	virtual ~TrainingScene();
	virtual void Update(float dt);
	ANNTrainer& GetANNTrainer() const;
	virtual bool IsTraining() const;
	virtual bool IsRunning() const;
	PhysicsManager & GetPhysicsManager() const;

	void SetNormalSpeed(bool set);
	bool GetNormalSpeed() const;

	void StopScene();

protected:
	virtual void UpdateMainEntity(float dt);
	virtual void UpdateBulletEntity(float dt);
	virtual void ContactEnterCallback(const ContactInfo & contactInfo);
	virtual void ContactExitCallback(const ContactInfo & contactInfo);

	virtual void StartGame();
	virtual void RestartGame();
	virtual void EndGame();

	bool							m_isRunning;
	bool							m_isPlayerOnGround;
	bool							m_gameRestarting;

	std::unique_ptr<PhysicsManager> m_physicsMgr;

	std::shared_ptr<PhysicsBody>	m_mainEntity;
	std::shared_ptr<PhysicsBody>	m_bulletEntity;

private:
	Randomizer						m_randomizer, m_bulletSpdRandomizer;
	std::unique_ptr<ANNTrainer>		m_annTrainer;
	std::unique_ptr<JumpData>		m_jumpData;
	unsigned						m_maxTrainingCount;
	float							m_jumpTimer;
	bool							m_normalSpd;
};