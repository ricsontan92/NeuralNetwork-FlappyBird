#include "PhysicsBody.h"
#include "PhysicsManager.h"

PhysicsBody::PhysicsBody(b2Body * body) :
	m_body(body),
	m_name("Default"),
	m_destroyed(false)
{
}

void PhysicsBody::AddForceToCenter(const math::vec2 & force)
{
	m_body->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
}

void PhysicsBody::AddAngularImpulse(float degree)
{
	m_body->ApplyAngularImpulse(DEG_TO_RAD(-degree) * m_body->GetInertia(), true);
}

void PhysicsBody::SetName(const std::string& name)
{
	m_name = name;
}

void PhysicsBody::SetFriction(float friction)
{
	b2Fixture * fixtures = m_body->GetFixtureList();
	while (fixtures)
	{
		fixtures->SetFriction(friction);
		fixtures = fixtures->GetNext();
	}
}

void PhysicsBody::SetGravityScale(float set)
{
	m_body->SetGravityScale(set);
}

void PhysicsBody::SetDensity(float set)
{
	b2Fixture * fixtures = m_body->GetFixtureList();
	while (fixtures)
	{
		fixtures->SetDensity(set);
		fixtures = fixtures->GetNext();
	}
	m_body->ResetMassData();
}

void PhysicsBody::Destroy()
{
	m_destroyed = true;
}

// gettors
const std::string & PhysicsBody::GetName() const 
{
	return m_name;
}

float PhysicsBody::GetFriction() const
{
	b2Fixture * fixtures = m_body->GetFixtureList();
	if (fixtures)
		return fixtures->GetDensity();
	else
		return 0.f;
}

float PhysicsBody::GetGravityScale() const
{
	return m_body->GetGravityScale();
}

math::vec2 PhysicsBody::GetPosition() const
{
	b2Vec2 pos = m_body->GetPosition();
	return math::vec2(pos.x, pos.y) * PhysicsManager::BOX2D_SCALE_FACTOR;
}

math::vec2 PhysicsBody::GetVelocity() const
{
	b2Vec2 spd = m_body->GetLinearVelocity();
	return math::vec2(spd.x, spd.y) * PhysicsManager::BOX2D_SCALE_FACTOR;
}

bool PhysicsBody::IsDestroyed() const
{
	return m_destroyed;
}