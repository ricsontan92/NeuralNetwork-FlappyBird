#include "PhysicsBody.h"
#include "PhysicsManager.h"
#include "DebugDrawer.h"

PhysicsBody::PhysicsBody(b2Body * body) :
	m_body(body),
	m_name("Default"),
	m_destroyed(false),
	m_categoryBits(0x0001),
	m_maskBits(0xFFFF),
	m_userData(nullptr),
	m_debugFill(false),
	m_debugColor(DEBUG_RED)
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

void PhysicsBody::SetCategoryBits(uint16 bits)
{
	SetFilterBits(bits, m_maskBits);
}

void PhysicsBody::SetMaskBits(uint16 bits)
{
	SetFilterBits(m_categoryBits, bits);
}

void PhysicsBody::SetIsSensor(bool set)
{
	b2Fixture * fixtures = m_body->GetFixtureList();
	while (fixtures)
	{
		fixtures->SetSensor(set);
		fixtures = fixtures->GetNext();
	}
}

void PhysicsBody::SetFilterBits(uint16 categoryBits, uint16 maskBits)
{
	b2Fixture * fixture = m_body->GetFixtureList();
	while (fixture)
	{
		b2Filter filter;
		filter.groupIndex	= 0;
		filter.categoryBits = categoryBits;
		filter.maskBits		= maskBits;

		fixture->SetFilterData(filter);
		fixture = fixture->GetNext();
	}

	m_maskBits		= maskBits;
	m_categoryBits	= categoryBits;
}

void PhysicsBody::SetVelocity(const math::vec2& velocity)
{
	m_body->SetLinearVelocity(b2Vec2(velocity.x * PhysicsManager::INV_BOX2D_SCALE_FACTOR, velocity.y * PhysicsManager::INV_BOX2D_SCALE_FACTOR));
}

void PhysicsBody::SetUserData(void * data)
{
	m_userData = data;
}

void PhysicsBody::SetDebugFill(bool set)
{
	m_debugFill = set;
}

void PhysicsBody::SetDebugColor(const math::vec4& color)
{
	m_debugColor = color;
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

math::vec2 PhysicsBody::GetSize() const
{
	return m_scale;
}

math::vec2 PhysicsBody::GetVelocity() const
{
	b2Vec2 spd = m_body->GetLinearVelocity();
	return math::vec2(spd.x, spd.y) * PhysicsManager::BOX2D_SCALE_FACTOR;
}

void* PhysicsBody::GetUserData() const
{
	return m_userData;
}

bool PhysicsBody::IsDestroyed() const
{
	return m_destroyed;
}

bool PhysicsBody::GetDebugFill() const
{
	return m_debugFill;
}

const math::vec4& PhysicsBody::GetDebugColor() const
{
	return m_debugColor;
}