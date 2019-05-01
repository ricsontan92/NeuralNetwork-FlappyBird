#pragma once

#include <ostream>

namespace math
{
	class vec3;
	class vec4
	{
	public:
		union
		{
			struct { float x, y, z, w; };
			float data[4];
		};

		vec4();
		vec4(float x, float y, float z, float w);
		vec4(math::vec3 const&, float w);
		~vec4();

		float LenSq() const;
		float Len() const;

		float Dot(const vec4& rhs) const;
		float DistSq(const vec4& rhs) const;
		float Dist(const vec4& rhs) const;
		void Normalize();
		vec4 GetNormalize() const;

		vec4 operator-()const;
		vec4 operator-(const vec4& rhs)const;
		vec4 operator+(const vec4& rhs)const;
		vec4 operator*(float rhs)const;
		vec4 operator/(float rhs)const;
	};
}