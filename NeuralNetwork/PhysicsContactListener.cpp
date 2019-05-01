#include "PhysicsContactListener.h"
#include "PhysicsBody.h"
PhysicsContactListener::PhysicsContactListener()
{

}

void PhysicsContactListener::BeginContact(b2Contact* contact)
{
	if(m_begContactFnc)
		(*m_begContactFnc)(GetContactInfo(contact));
}

void PhysicsContactListener::EndContact(b2Contact* contact)
{
	if(m_endContactFnc)
		(*m_endContactFnc)(GetContactInfo(contact));
}

void PhysicsContactListener::ClearListenerFunctions()
{
	m_begContactFnc.reset();
	m_endContactFnc.reset();
}

ContactInfo PhysicsContactListener::GetContactInfo(b2Contact* contact)
{
	b2WorldManifold manifold;
	contact->GetWorldManifold(&manifold);

	ContactInfo info;
	info.m_bodyA = reinterpret_cast<PhysicsBody*>(contact->GetFixtureA()->GetBody()->GetUserData());
	info.m_bodyB = reinterpret_cast<PhysicsBody*>(contact->GetFixtureB()->GetBody()->GetUserData());
	info.m_isTouching = contact->IsTouching();
	info.m_contactWorldNormal = math::vec2(manifold.normal.x, manifold.normal.y);
	info.m_contactWorldPoints[0] = math::vec2(manifold.points[0].x, manifold.points[0].y);
	info.m_contactWorldPoints[1] = math::vec2(manifold.points[1].x, manifold.points[1].y);

	return info;
}