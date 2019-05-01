#pragma once

#include "Box2D/box2d.h"
#include "math.h"

#include <string>

class PhysicsBody
{
public:
	PhysicsBody(b2Body * body);

	// manipulators
	void AddForceToCenter(const math::vec2 & force);
	void AddAngularImpulse(float degree);

	// settors
	void SetName(const std::string& name);
	void SetFriction(float friction);
	void SetGravityScale(float set);
	void SetDensity(float set);
	void Destroy();

	// gettors
	const std::string & GetName() const;
	float GetFriction() const;
	float GetGravityScale() const;
	math::vec2 GetPosition() const;
	math::vec2 GetVelocity() const;
	bool IsDestroyed() const;

private:
	friend class PhysicsManager;

	bool		m_destroyed;
	b2Body *	m_body;
	std::string	m_name;
};
