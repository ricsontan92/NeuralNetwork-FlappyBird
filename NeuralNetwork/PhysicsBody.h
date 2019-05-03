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
	void SetCategoryBits(uint16 bits);
	void SetMaskBits(uint16 bits);
	void SetIsSensor(bool set);
	void SetFilterBits(uint16 categoryBits, uint16 maskBits);
	void SetVelocity(const math::vec2& velocity);
	void SetUserData(void * data);
	void SetDebugFill(bool set);
	void SetDebugColor(const math::vec4& color);
	void Destroy();

	// gettors
	const std::string & GetName() const;
	float GetFriction() const;
	float GetGravityScale() const;
	math::vec2 GetPosition() const;
	math::vec2 GetSize() const;
	math::vec2 GetVelocity() const;
	void* GetUserData() const;
	bool IsDestroyed() const;
	bool GetDebugFill() const;
	const math::vec4& GetDebugColor() const;
private:
	friend class PhysicsManager;

	uint16		m_categoryBits;
	uint16		m_maskBits;

	bool		m_destroyed;
	b2Body *	m_body;
	std::string	m_name;
	math::vec2	m_scale;

	void*		m_userData;
	bool		m_debugFill;
	math::vec4	m_debugColor;
};
