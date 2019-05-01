#pragma once
#include "vec3.h"
#include "mat4.h"

namespace math
{
	class quat
	{
	public:
		quat();
		quat(float x, float y, float z, float w);
		quat(float deg, const vec3& v);
		~quat();

		quat GetInverse() const;
		quat GetConjugate() const;

		quat& Normalize();
		quat GetNormalize() const;

		void GetAxisAngle(float& angle, vec3& axis) const;

		float LenSq() const;
		float Len() const;
		float Dot(const quat& q) const;
		mat4 ToMat4() const;
		
		quat operator*(const quat& rhs) const;
		quat operator+(const quat& rhs) const;

		quat operator*(float rhs) const;

		quat& operator*=(const quat& rhs);

	private:
		float scalar;
		vec3  vec;
	};
}
