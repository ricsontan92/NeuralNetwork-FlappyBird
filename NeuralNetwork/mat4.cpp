#include "mat4.h"
#include "vec3.h"
#include <cmath>
#include <algorithm>

namespace math
{
	mat4::mat4() : m16{	0.f, 0.f, 0.f, 0.f, 
						0.f, 0.f, 0.f, 0.f, 
						0.f, 0.f, 0.f, 0.f,
						0.f, 0.f, 0.f, 0.f }
	{
	}

	mat4::~mat4()
	{
	}

	void mat4::Transpose()
	{
		std::swap(m44[0][1], m44[1][0]);
		std::swap(m44[0][2], m44[2][0]);
		std::swap(m44[0][3], m44[3][0]);

		std::swap(m44[1][2], m44[2][1]);
		std::swap(m44[1][3], m44[3][1]);

		std::swap(m44[2][3], m44[3][2]);
	}

	mat4 mat4::GetTranspose() const
	{
		mat4 c(*this);
		c.Transpose();
		return c;
	}

	mat4 mat4::operator*(float rhs) const
	{
		mat4 c(*this);
		for (auto& elem : c.m16)
			elem *= rhs;
		return c;
	}

	mat4 mat4::operator*(const mat4& rhs) const
	{
		mat4 c;
		for (int x = 0; x < 4; x++) 
			for (int y = 0; y < 4; y++) 
				for (int z = 0; z < 4; z++) 
					c.m44[x][y] += m44[x][z] * rhs.m44[z][y];
		return c;
	}

	vec3 mat4::operator*(const vec3& rhs) const
	{
		vec3 result(rhs);
		result.x = m44[0][0] * rhs.x + m44[0][1] * rhs.y + m44[0][2] * rhs.z + m44[0][3];
		result.y = m44[1][0] * rhs.x + m44[1][1] * rhs.y + m44[1][2] * rhs.z + m44[1][3];
		result.z = m44[2][0] * rhs.x + m44[2][1] * rhs.y + m44[2][2] * rhs.z + m44[2][3];
		return result;
	}

	mat4 mat4::GetIdentity()
	{
		mat4 m;
		m.m44[0][0] = 1.f;
		m.m44[1][1] = 1.f;
		m.m44[2][2] = 1.f;
		m.m44[3][3] = 1.f;
		return m;
	}

	mat4 mat4::Translate(const math::vec3& rhs)
	{
		mat4 c = GetIdentity();
		c.m44[0][3] = rhs.x;
		c.m44[1][3] = rhs.y;
		c.m44[2][3] = rhs.z;
		return c;
	}

	mat4 mat4::Scale(const math::vec3& rhs)
	{
		mat4 c;
		c.m44[0][0] = rhs.x;
		c.m44[1][1] = rhs.y;
		c.m44[2][2] = rhs.z;
		c.m44[3][3] = 1.f;
		return c;
	}

	mat4 mat4::Scale(float rhs)
	{
		mat4 c;
		c.m44[0][0] = rhs;
		c.m44[1][1] = rhs;
		c.m44[2][2] = rhs;
		c.m44[3][3] = 1.f;
		return c;
	}

	mat4 mat4::Rotate(float deg, const math::vec3& axis)
	{
		math::vec3 norm_axis = axis.GetNormalize();

		float cosTheta = cosf(deg * 0.0174533f);
		float sinTheta = sinf(deg * 0.0174533f);
		float one_minus_cos = 1.f - cosTheta;
		mat4 c;

		c.m44[0][0] = norm_axis.x * norm_axis.x * one_minus_cos + cosTheta;
		c.m44[0][1] = norm_axis.x * norm_axis.y * one_minus_cos - norm_axis.z * sinTheta;
		c.m44[0][2] = norm_axis.x * norm_axis.z * one_minus_cos + norm_axis.y * sinTheta;

		c.m44[1][0] = norm_axis.x * norm_axis.y * one_minus_cos + norm_axis.z * sinTheta;
		c.m44[1][1] = norm_axis.y * norm_axis.y * one_minus_cos + cosTheta;
		c.m44[1][2] = norm_axis.y * norm_axis.z * one_minus_cos - norm_axis.x * sinTheta;

		c.m44[2][0] = norm_axis.x * norm_axis.z * one_minus_cos - norm_axis.y * sinTheta;
		c.m44[2][1] = norm_axis.y * norm_axis.z * one_minus_cos + norm_axis.x * sinTheta;
		c.m44[2][2] = norm_axis.z * norm_axis.z * one_minus_cos + cosTheta;

		c.m44[3][3] = 1.f;

		return c;
	}

	mat4 mat4::Rotate2D(float deg)
	{
		mat4 c = GetIdentity();
		float rad = deg * 0.0174533f;
		c.m44[1][1] = c.m44[0][0] = cosf(rad);
		c.m44[0][1] = -sinf(rad);
		c.m44[1][0] = -c.m44[0][1];
		return c;
	}

	mat4 mat4::Perspective(float deg, float aspect, float nr, float fr)
	{
		float const rad = deg * 0.0174533f;
		float const tanHalfFovy = tan(rad * 0.5f);

		mat4 c;
		c.m44[0][0] = 1.f / (aspect * tanHalfFovy);
		c.m44[1][1] = 1.f / tanHalfFovy;
		c.m44[2][2] = -(fr + nr) / (fr - nr);
		c.m44[3][2] = -1.f;
		c.m44[2][3] = -(2.f * fr * nr) / (fr - nr);

		return c;
	}

	mat4 mat4::Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
	{
		mat4 c = GetIdentity();

		c.m44[0][0] = 2.f / (right - left);
		c.m44[1][1] = 2.f / (top - bottom);
		c.m44[2][2] = 2.f / (zNear - zFar);

		c.m44[0][3] = -(right + left) / (right - left);
		c.m44[1][3] = -(top + bottom) / (top - bottom);
		c.m44[2][3] = -(zFar + zNear) / (zFar - zNear);

		return c;
	}

	void mat4::GetAxisAngle(float& angle, math::vec3& axis) const
	{
		const mat4& m = *this;

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

	mat4 mat4::LookAt(const math::vec3& pos, const math::vec3& target, const math::vec3& up)
	{
		vec3 const f = (target - pos).GetNormalize();
		vec3 const s = f.Cross(up).GetNormalize();
		vec3 const u = s.Cross(f);

		mat4 c = GetIdentity();
		
		c.m44[0][0] = s.x;
		c.m44[0][1] = s.y;
		c.m44[0][2] = s.z;
		c.m44[1][0] = u.x;
		c.m44[1][1] = u.y;
		c.m44[1][2] = u.z;
		c.m44[2][0] = -f.x;
		c.m44[2][1] = -f.y;
		c.m44[2][2] = -f.z;
		c.m44[0][3] = -s.Dot(pos);
		c.m44[1][3] = -u.Dot(pos);
		c.m44[2][3] = f.Dot(pos);

		return c;
	}
}