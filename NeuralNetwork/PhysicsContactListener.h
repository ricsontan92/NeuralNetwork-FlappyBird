#pragma once

#include "Box2D/box2d.h"
#include "math.h"

class PhysicsBody;

struct ContactInfo
{
	PhysicsBody*	m_bodyA;
	PhysicsBody*	m_bodyB;
	math::vec2		m_contactWorldNormal;
	math::vec2		m_contactWorldPoints[2];
	bool			m_isTouching;
};

class PhysicsContactListener : public b2ContactListener
{
	struct Concept
	{
		virtual void operator()(const ContactInfo& info) = 0;
	};

	template<typename ... Args>
	struct Model : Concept {};

	template<typename F, typename C>
	struct Model<F, C> : Concept
	{
		Model(F f, C c) : m_function(f), m_fncClass(c) {}

		F m_function;
		C m_fncClass;
		void operator()(const ContactInfo& info)
		{
			(m_fncClass->*m_function)(info);
		}
	};

public:
	PhysicsContactListener();

	template<typename F, typename C> void SetBeginContactCallbackFunction(F fnc, C fncClass);
	template<typename F, typename C> void SetEndContactCallbackFunction(F fnc, C fncClass);
	template<typename F> void SetBeginContactCallbackFunction(F fnc);
	template<typename F> void SetEndContactCallbackFunction(F fnc);
	void ClearListenerFunctions();

	// Called when two fixtures begin to touch
	virtual void BeginContact(b2Contact* contact);

	// Called when two fixtures cease to touch
	virtual void EndContact(b2Contact* contact);

private:
	ContactInfo GetContactInfo(b2Contact* contact);
	std::unique_ptr<Concept> m_begContactFnc;
	std::unique_ptr<Concept> m_endContactFnc;
};

template<typename F, typename C>
void PhysicsContactListener::SetBeginContactCallbackFunction(F fnc, C fncClass)
{
	m_begContactFnc = std::make_unique<Model<F, C >>(fnc, fncClass);
}

template<typename F, typename C>
void PhysicsContactListener::SetEndContactCallbackFunction(F fnc, C fncClass)
{
	m_endContactFnc = std::make_unique<Model<F, C >>(fnc, fncClass);
}

template<typename F> 
void PhysicsContactListener::SetBeginContactCallbackFunction(F fnc)
{
	m_begContactFnc = std::make_unique<Model<F>>(fnc);
}

template<typename F> 
void PhysicsContactListener::SetEndContactCallbackFunction(F fnc)
{
	m_endContactFnc = std::make_unique<Model<F>>(fnc);
}