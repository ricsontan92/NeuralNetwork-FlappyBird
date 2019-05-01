#include "vec4.h"
#include "vec3.h"
#include <cmath>
#include <float.h>

namespace math
{
	vec4::vec4() : x(0.f), y(0.f), z(0.f), w(0.f)
	{
	}

	vec4::~vec4()
	{
	}

	vec4::vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
	{

	}

	vec4::vec4(math::vec3 const& v3, float _w) : vec4(v3.x, v3.y, v3.z, _w)
	{

	}

	float vec4::LenSq() const
	{
		return x*x + y*y + z*z + w*w;
	}

	float vec4::Len() const
	{
		return sqrtf(LenSq());
	}

	float vec4::Dot(const vec4& rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
	}

	float vec4::DistSq(const vec4& rhs) const
	{
		return Dot(rhs);
	}

	float vec4::Dist(const vec4& rhs) const
	{
		return sqrtf(Dot(rhs));
	}

	void vec4::Normalize()
	{
		float l = LenSq();
		if (l > FLT_EPSILON)
			*this = *this / sqrt(l);
	}

	vec4 vec4::GetNormalize() const
	{
		vec4 c(*this);
		c.Normalize();
		return c;
	}

	vec4 vec4::operator-()const
	{
		vec4 c(*this);
		c.x = -c.x;
		c.y = -c.y;
		c.z = -c.z;
		c.w = -c.w;
		return c;
	}

	vec4 vec4::operator-(const vec4& rhs)const
	{
		vec4 c(*this);
		c.x -= rhs.x;
		c.y -= rhs.y;
		c.z -= rhs.z;
		c.w -= rhs.w;
		return c;
	}

	vec4 vec4::operator+(const vec4& rhs)const
	{
		vec4 c(*this);
		c.x += rhs.x;
		c.y += rhs.y;
		c.z += rhs.z;
		c.w += rhs.w;
		return c;
	}

	vec4 vec4::operator*(float rhs)const
	{
		vec4 c(*this);
		c.x *= rhs;
		c.y *= rhs;
		c.z *= rhs;
		c.w *= rhs;
		return c;
	}

	vec4 vec4::operator/(float rhs)const
	{
		vec4 c(*this);
		c.x /= rhs;
		c.y /= rhs;
		c.z /= rhs;
		c.w /= rhs;
		return c;
	}
}