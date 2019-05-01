#include "quat.h"
#include "mat4.h"
#include <cmath>

namespace math
{
	quat::quat() : scalar(1.f), vec()
	{
	}

	quat::quat(float x, float y, float z, float w) : scalar(x), vec(y, z, w)
	{

	}

	quat::quat(float deg, const vec3& v) : vec(v.GetNormalize())
	{
		float half_rad = deg * 0.0174533f * 0.5f;
		scalar	 = cosf(half_rad);
		vec		*= sinf(half_rad);
	}

	quat::~quat()
	{
	}

	float quat::LenSq() const
	{
		return scalar * scalar + vec.LenSq();
	}

	float quat::Len() const
	{
		return sqrt(LenSq());
	}

	float quat::Dot(const quat& q) const
	{
		return scalar * q.scalar + vec.Dot(q.vec);
	}

	mat4 quat::ToMat4() const
	{
		mat4 c = mat4::GetIdentity();

		const float& q0 = scalar;
		const float& q1 = vec.x;
		const float& q2 = vec.y;
		const float& q3 = vec.z;

		float q0q0 = q0 * q0;
		float q1q1 = q1 * q1;
		float q2q2 = q2 * q2;
		float q3q3 = q3 * q3;

		float q1q3 = q1 * q3;
		float q1q2 = q1 * q2;
		float q0q1 = q0 * q1;
		float q0q3 = q0 * q3;
		float q0q2 = q0 * q2;
		float q2q3 = q2 * q3;

		c.m44[0][0] = q0q0 + q1q1 - q2q2 - q3q3;
		c.m44[0][1] = 2.f * (q1q2 - q0q3);
		c.m44[0][2] = 2.f * (q1q3 + q0q2);

		c.m44[1][0] = 2.f * (q1q2 + q0q3);
		c.m44[1][1] = q0q0 - q1q1 + q2q2 - q3q3;
		c.m44[1][2] = 2.f * (q2q3 - q0q1);

		c.m44[2][0] = 2.f * (q1q3 - q0q2);
		c.m44[2][1] = 2.f * (q2q3 + q0q1);
		c.m44[2][2] = q0q0 - q1q1 - q2q2 + q3q3;

		return c;
	}

	quat& quat::Normalize()
	{
		float len = Len();
		scalar /= len;
		vec /= len;
		return *this;
	}

	quat quat::GetNormalize() const
	{
		quat c(*this);
		c.Normalize();
		return c;
	}

	quat quat::GetInverse() const
	{
		quat c = GetConjugate();
		return c.GetNormalize();
	}

	quat quat::GetConjugate() const
	{
		quat c;
		c.scalar = scalar;
		c.vec = -vec;
		return c;
	}

	quat quat::operator*(const quat& rhs) const
	{
		quat c(*this);
		c *= rhs;
		return c;
	}

	quat quat::operator+(const quat& rhs) const
	{
		quat c(*this);
		c.scalar += rhs.scalar;
		c.vec += rhs.vec;
		return c;
	}

	quat quat::operator*(float rhs) const
	{
		quat c(*this);
		c.scalar *= rhs;
		c.vec *= rhs;
		return c;
	}

	quat& quat::operator*=(const quat& rhs)
	{
		float temp = scalar;
		scalar = scalar * rhs.scalar - vec.Dot(rhs.vec);
		vec = rhs.vec * temp + vec * rhs.scalar + vec.Cross(rhs.vec);
		return *this;
	}

	void quat::GetAxisAngle(float& angle, vec3& axis) const
	{
		mat4 m = ToMat4();

		float trace = m.m44[0][0] + m.m44[1][1] + m.m44[2][2];

		// trace = 1 + 2 cos(theta)
		// acos ( (trace - 1) * 0.5f )
		float rad = acosf((trace - 1.f) * 0.5f);

		angle = rad * 57.2957795f;	// convert to degrees

 		axis.x = m.m44[2][1] - m.m44[1][2];
		axis.y = m.m44[0][2] - m.m44[2][0];
		axis.z = m.m44[1][0] - m.m44[0][1];
		axis /= (2.f * sinf(rad));
	}
}