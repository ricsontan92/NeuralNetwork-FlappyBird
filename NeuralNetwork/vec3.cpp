#include "vec3.h"
#include "vec2.h"
#include <cmath>
#include <float.h>

namespace math
{
	vec3::vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{
	}

	vec3::vec3() : x(0.f), y(0.f), z(0.f)
	{
	}

	vec3::~vec3()
	{
	}

	math::vec2 vec3::xy() const
	{
		return math::vec2(x, y);
	}

	math::vec2 vec3::yz() const
	{
		return math::vec2(y, z);
	}

	math::vec2 vec3::xz() const
	{
		return math::vec2(x, z);
	}

	float vec3::LenSq() const
	{
		return x*x + y*y + z*z;
	}

	float vec3::Len() const
	{
		return sqrtf(LenSq());
	}

	float vec3::Dot(const vec3& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	float vec3::DistSq(const vec3& rhs) const
	{
		float xDiff = x - rhs.x;
		float yDiff = y - rhs.y;

		return xDiff * xDiff + yDiff * yDiff;
	}

	float vec3::Dist(const vec3& rhs) const
	{
		return sqrtf(DistSq(rhs));
	}

	void vec3::Normalize()
	{
		float l = LenSq();
		if(l > FLT_EPSILON)
			*this = *this / sqrt(l);
	}

	vec3 vec3::GetNormalize() const
	{
		vec3 c(*this);
		c.Normalize();
		return c;
	}

	vec3 vec3::GetAbsolute() const
	{
		vec3 c(fabs(x), fabs(y), fabs(z));
		return c;
	}

	vec3 vec3::Cross(const vec3& rhs) const
	{
		vec3 c(*this);
		c.x = y * rhs.z - z * rhs.y;
		c.y = z * rhs.x - x * rhs.z;
		c.z = x * rhs.y - y * rhs.x;
		return c;
	}

	vec3 vec3::operator-()const
	{
		vec3 c(*this);
		c.x = -c.x;
		c.y = -c.y;
		c.z = -c.z;
		return c;
	}

	vec3 vec3::operator-(const vec3& rhs)const
	{
		vec3 c(*this);
		c.x -= rhs.x;
		c.y -= rhs.y;
		c.z -= rhs.z;
		return c;
	}

	vec3 vec3::operator+(const vec3& rhs)const
	{
		vec3 c(*this);
		c.x += rhs.x;
		c.y += rhs.y;
		c.z += rhs.z;
		return c;
	}

	vec3 vec3::operator*(float rhs)const
	{
		vec3 c(*this);
		c.x *= rhs;
		c.y *= rhs;
		c.z *= rhs;
		return c;
	}

	vec3 vec3::operator/(float rhs)const
	{
		vec3 c(*this);
		c.x /= rhs;
		c.y /= rhs;
		c.z /= rhs;
		return c;
	}

	vec3 vec3::operator/(const vec3& rhs)const
	{
		vec3 c(*this);
		c.x /= rhs.x;
		c.y /= rhs.y;
		c.z /= rhs.z;
		return c;
	}

	vec3 vec3::operator*(const vec3& rhs)const
	{
		vec3 c(*this);
		c.x *= rhs.x;
		c.y *= rhs.y;
		c.z *= rhs.z;
		return c;
	}

	vec3& vec3::operator+=(const vec3& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	vec3& vec3::operator-=(const vec3& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	vec3& vec3::operator/=(float rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	vec3& vec3::operator*=(float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	float& vec3::operator[](int index)
	{
		return data[index];
	}

	float vec3::operator[](int index) const
	{
		return data[index];
	}

	bool vec3::operator==(const vec3& rhs) const
	{
		if (x != rhs.x) return false;
		if (y != rhs.y) return false;
		if (z != rhs.z) return false;
		return true;
	}

	bool vec3::operator!=(const vec3& rhs) const
	{
		if (x != rhs.x) return true;
		if (y != rhs.y) return true;
		if (z != rhs.z) return true;
		return false;
	}

	const vec3& vec3::XAxis()
	{
		static const vec3 axis(1.f, 0.f, 0.f);
		return axis;
	}

	const vec3& vec3::YAxis()
	{
		static const vec3 axis(0.f, 1.f, 0.f);
		return axis;
	}

	const vec3& vec3::ZAxis()
	{
		static const vec3 axis(0.f, 0.f, 1.f);
		return axis;
	}

	std::ostream& operator<<(std::ostream& os, const math::vec3& val)
	{
		os << val.x << ", " << val.y << ", " << val.z;
		return os;
	}
}