#include "PhysicsManager.h"

#include "PhysicsContactListener.h"
#include "DebugDrawer.h"
#include "PhysicsBody.h"

const float PhysicsManager::BOX2D_SCALE_FACTOR = 50.f;
const float PhysicsManager::INV_BOX2D_SCALE_FACTOR = 1.f / PhysicsManager::BOX2D_SCALE_FACTOR;

b2Vec2 operator*(const b2Vec2 & rhs, float scalar)
{
	return b2Vec2(rhs.x * scalar, rhs.y * scalar);
}

PhysicsManager::PhysicsManager(const math::vec2 & gravity, DebugDrawer& debugDrawer) :
		m_world(std::make_unique<b2World>(b2Vec2(gravity.x, gravity.y))),
		m_debugDrawer(debugDrawer),
		m_listener(std::make_unique<PhysicsContactListener>())
{
	m_world->SetContactListener(&(*m_listener));
}

PhysicsManager::~PhysicsManager()
{
}

void PhysicsManager::Update(float dt, int velocityIter, int positionIter)
{
	m_world->Step(dt, velocityIter, positionIter);
	ClearDestroyedShapes();
}

void PhysicsManager::RenderDebugShapes() const
{
	std::lock_guard<std::mutex> lck(m_physicsBodiesMtx);
	for (auto const& physicBody : m_physicsBodies)
	{
		b2Fixture * pFixture = physicBody->m_body->GetFixtureList();

		while (pFixture)
		{
			b2Vec2 pos		= physicBody->m_body->GetPosition() * BOX2D_SCALE_FACTOR;
			float radians	= RAD_TO_DEG(physicBody->m_body->GetAngle());

			switch (pFixture->GetType())
			{
			case b2Shape::e_circle:
			{
				float radius = static_cast<b2CircleShape*>(pFixture->GetShape())->m_radius * BOX2D_SCALE_FACTOR;
				math::mat4 transform =	math::mat4::Translate(math::vec3(pos.x, pos.y, 0.f)) *
										math::mat4::Rotate2D(radians) *
										math::mat4::Scale(radius);
				if(physicBody->GetDebugFill())
					m_debugDrawer.AddDebugFilledCircle(transform, physicBody->m_debugColor);
				else
					m_debugDrawer.AddDebugCircle(transform, physicBody->m_debugColor);
			}
			case b2Shape::e_polygon:
			{
				b2Vec2 dim =	(static_cast<b2PolygonShape*>(pFixture->GetShape())->m_vertices[0] -
								static_cast<b2PolygonShape*>(pFixture->GetShape())->m_centroid) * BOX2D_SCALE_FACTOR * 2.f;
				math::mat4 transform =	math::mat4::Translate(math::vec3(pos.x, pos.y, 0.f)) *
										math::mat4::Rotate2D(radians) *
										math::mat4::Scale(math::vec3(fabs(dim.x), fabs(dim.y), 0.f));

				if (physicBody->GetDebugFill())
					m_debugDrawer.AddFilledDebugBox(transform, physicBody->m_debugColor);
				else
					m_debugDrawer.AddDebugBox(transform, physicBody->m_debugColor);
			}
			case b2Shape::e_chain:
			case b2Shape::e_edge:
				break;
			}

			pFixture = pFixture->GetNext();
		}
	}
}

void PhysicsManager::ClearDestroyedShapes()
{
	std::lock_guard<std::mutex> lck(m_physicsBodiesMtx);
	for (auto it = m_physicsBodies.begin(); it != m_physicsBodies.end();)
	{
		PhysicsBody & obj = **it;
		if (obj.IsDestroyed() && obj.m_body)
		{
			m_world->DestroyBody(obj.m_body);
			obj.m_body = nullptr;
			it = m_physicsBodies.erase(it);
		}
		else
		{
			++it;
		}
	}
	
}

b2Body* PhysicsManager::CreateBody(const math::vec2 & pos, float radians, b2BodyType type) const
{
	b2BodyDef bodyDef;
	bodyDef.position.Set(pos.x * INV_BOX2D_SCALE_FACTOR, pos.y * INV_BOX2D_SCALE_FACTOR);
	bodyDef.angle	= radians;
	bodyDef.type	= type;
	bodyDef.fixedRotation = false;

	return m_world->CreateBody(&bodyDef);
}

b2Fixture* PhysicsManager::CreateFixture(b2Body* body, const b2Shape& shape) const
{
	b2FixtureDef fixtureDef;
	fixtureDef.shape		= &shape;
	fixtureDef.density		= 1.f;	// since its area with respect to density
	fixtureDef.friction		= 1.f;
	fixtureDef.restitution	= 0.f;
	return body->CreateFixture(&fixtureDef);
}

void PhysicsManager::CreatePhysicsBody(b2Body* body)
{
	std::lock_guard<std::mutex> lck(m_physicsBodiesMtx);
	m_physicsBodies.emplace_back(std::make_shared<PhysicsBody>(body));
	body->SetUserData(&(*m_physicsBodies.back()));
}

PhysicBodyPtr PhysicsManager::AddCircle(const math::vec2 & pos, float angle, float radius, BodyType bodyType)
{
	b2CircleShape shape;
	shape.m_radius	= radius * INV_BOX2D_SCALE_FACTOR;

	b2Body * pBody = CreateBody(pos, DEG_TO_RAD(angle), b2BodyType(bodyType));
	CreatePhysicsBody(CreateFixture(pBody, shape)->GetBody());
	m_physicsBodies.back()->m_scale = math::vec2(radius, radius);
	return m_physicsBodies.back();
}

PhysicBodyPtr PhysicsManager::AddBox(const math::vec2 & pos, const math::vec2& size, float angle, BodyType bodyType)
{
	b2PolygonShape shape;
	shape.SetAsBox(size.x * INV_BOX2D_SCALE_FACTOR * 0.5f, size.y * INV_BOX2D_SCALE_FACTOR * 0.5f);

	b2Body * pBody = CreateBody(pos, DEG_TO_RAD(angle), b2BodyType(bodyType));
	CreatePhysicsBody(CreateFixture(pBody, shape)->GetBody());
	m_physicsBodies.back()->m_scale = size;
	return m_physicsBodies.back();
}

PhysicsContactListener& PhysicsManager::GetContactListener()
{
	return *m_listener;
}

const PhysicsContactListener& PhysicsManager::GetContactListener() const
{
	return *m_listener;
}

const std::vector<PhysicBodyPtr>& PhysicsManager::GetAllBodies() const
{
	std::lock_guard<std::mutex> lck(m_physicsBodiesMtx);
	return m_physicsBodies;
}

void PhysicsManager::Clear()
{
	std::lock_guard<std::mutex> lck(m_physicsBodiesMtx);
	for (auto & body : m_physicsBodies)
	{
		m_world->DestroyBody(body->m_body);
	}
	m_physicsBodies.clear();
}