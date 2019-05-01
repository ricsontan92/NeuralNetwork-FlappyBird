#include "vec2.h"
#include "vec3.h"
#include <cmath>
#include <float.h>

namespace math
{
	vec2::vec2(float _x, float _y) : x(_x), y(_y)
	{
	}

	vec2::vec2() : x(0.f), y(0.f)
	{
	}

	vec2::~vec2()
	{
	}

	float vec2::LenSq() const
	{
		return x*x + y*y;
	}

	float vec2::Len() const
	{
		return sqrtf(LenSq());
	}

	float vec2::Dot(const vec2& rhs) const
	{
		return x * rhs.x + y * rhs.y;
	}

	float vec2::DistSq(const vec2& rhs) const
	{
		float xDiff = x - rhs.x;
		float yDiff = y - rhs.y;

		return xDiff * xDiff + yDiff * yDiff;
	}

	float vec2::Dist(const vec2& rhs) const
	{
		return sqrtf(DistSq(rhs));
	}

	void vec2::Normalize()
	{
		float l = LenSq();
		if(l > FLT_EPSILON)
			*this = *this / sqrt(l);
	}

	vec2 vec2::GetNormalize() const
	{
		vec2 c(*this);
		c.Normalize();
		return c;
	}

	vec2 vec2::RotateBy(float degrees) const
	{
		float radians = degrees * 3.14159f / 180.f;
		float cos_rad = cosf(radians);
		float sin_rad = sinf(radians);

		vec2 c;

		c.x = cos_rad * x - sin_rad * y;
		c.y = cos_rad * y + sin_rad * x;

		return c;
	}

	vec2 vec2::ProjectOnto(vec2 const& onto_axis) const
	{
		float a_dot_v	= Dot(onto_axis);
		float v_dot_v	= onto_axis.Dot(onto_axis);
		return onto_axis * (a_dot_v / v_dot_v);
	}

	vec2 vec2::operator-()const
	{
		vec2 c(*this);
		c.x = -c.x;
		c.y = -c.y;
		return c;
	}

	vec2 vec2::operator-(const vec2& rhs)const
	{
		vec2 c(*this);
		c.x -= rhs.x;
		c.y -= rhs.y;
		return c;
	}

	vec2 vec2::operator+(const vec2& rhs)const
	{
		vec2 c(*this);
		c.x += rhs.x;
		c.y += rhs.y;
		return c;
	}

	vec2 vec2::operator*(const vec2& rhs)const
	{
		vec2 c(*this);
		c.x *= rhs.x;
		c.y *= rhs.y;
		return c;
	}

	vec2 vec2::operator/(const vec2& rhs)const
	{
		vec2 c(*this);
		c.x /= rhs.x;
		c.y /= rhs.y;
		return c;
	}

	vec2 vec2::operator*(float rhs)const
	{
		vec2 c(*this);
		c.x *= rhs;
		c.y *= rhs;
		return c;
	}

	vec2 vec2::operator/(float rhs)const
	{
		vec2 c(*this);
		c.x /= rhs;
		c.y /= rhs;
		return c;
	}

	vec2& vec2::operator+=(const vec2& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	vec2& vec2::operator-=(const vec2& rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	vec2& vec2::operator/=(float rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	vec2& vec2::operator*=(float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	vec2::operator vec3() const
	{
		return vec3(x, y, 0.f);
	}

	float& vec2::operator[](int index)
	{
		return data[index];
	}

	float vec2::operator[](int index) const
	{
		return data[index];
	}

	bool vec2::operator==(const vec2& rhs) const
	{
		if (x != rhs.x) return false;
		if (y != rhs.y) return false;
		return true;
	}

	std::ostream& operator<<(std::ostream& os, const math::vec2& val)
	{
		os << val.x << ", " << val.y;
		return os;
	}

	vec2 operator/(float lhs, const vec2& rhs)
	{
		vec2 c(rhs);
		c.x = lhs / c.x;
		c.y = lhs / c.y;
		return c;
	}

	vec2 operator*(float lhs, const vec2& rhs)
	{
		vec2 c(rhs);
		c.x = lhs * c.x;
		c.y = lhs * c.y;
		return c;
	}
}

