#pragma once

#include "math.h"
#include "Box2D/box2d.h"

#include <vector>
#include <memory>
#include <mutex>

class b2World;
class DebugDrawer;
class PhysicsBody;
class PhysicsContactListener;

using PhysicBodyPtr = std::shared_ptr<PhysicsBody>;

class PhysicsManager
{
public:
	static const float BOX2D_SCALE_FACTOR;
	static const float INV_BOX2D_SCALE_FACTOR;

	enum class BodyType
	{
		DYNAMIC		= b2_dynamicBody,
		STATIC		= b2_staticBody,
		KINEMATIC	= b2_kinematicBody
	};

	PhysicsManager(const math::vec2 & gravity, DebugDrawer& debugDrawer);
	~PhysicsManager();
	void Update(float dt, int velocityIter = 8, int positionIter = 3);

	PhysicBodyPtr AddCircle(const math::vec2 & pos, float angle, float radius, BodyType bodyType);
	PhysicBodyPtr AddBox(const math::vec2 & pos, const math::vec2& size, float angle, BodyType bodyType);
	void RenderDebugShapes() const;

	PhysicsContactListener& GetContactListener();
	const PhysicsContactListener& GetContactListener() const;
	const std::vector<PhysicBodyPtr>& GetAllBodies() const;

	void Clear();

private:
	void ClearDestroyedShapes();
	b2Body* CreateBody(const math::vec2 & pos, float radians, b2BodyType type) const;
	b2Fixture* CreateFixture(b2Body* body, const b2Shape& shape) const;
	void CreatePhysicsBody(b2Body* body);

	b2Vec2						m_gravity;
	std::unique_ptr<b2World>	m_world;
	std::unique_ptr<PhysicsContactListener> m_listener;
	DebugDrawer&				m_debugDrawer;

	std::vector<std::shared_ptr<PhysicsBody>>	m_physicsBodies;
	mutable std::mutex							m_physicsBodiesMtx;
};