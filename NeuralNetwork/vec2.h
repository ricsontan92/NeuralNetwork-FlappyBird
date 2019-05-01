#pragma once

#include <ostream>

namespace math
{
	class vec3;

	class vec2
	{
	public:
		union
		{
			struct { float x, y; };
			float data[2];
		};

		vec2(float _x, float _y);
		vec2();
		~vec2();

		float LenSq() const;
		float Len() const;
		float Dot(const vec2& rhs) const;
		float DistSq(const vec2& rhs) const;
		float Dist(const vec2& rhs) const;
		void Normalize();
		vec2 GetNormalize() const;
		vec2 ProjectOnto(vec2 const&) const;
		vec2 RotateBy(float degrees) const;

		vec2 operator-()const;
		vec2 operator-(const vec2& rhs)const;
		vec2 operator+(const vec2& rhs)const;
		vec2 operator*(const vec2& rhs)const;
		vec2 operator/(const vec2& rhs)const;
		vec2 operator*(float rhs)const;
		vec2 operator/(float rhs)const;

		vec2& operator+=(const vec2& rhs);
		vec2& operator-=(const vec2& rhs);
		vec2& operator/=(float rhs);
		vec2& operator*=(float rhs);

		operator vec3() const;

		float& operator[](int index);
		float operator[](int index) const;

		bool operator==(const vec2& rhs) const;

		friend std::ostream& operator<<(std::ostream& os, const math::vec2& val);
	};

	vec2 operator/(float lhs, const vec2& rhs);
	vec2 operator*(float lhs, const vec2& rhs);
}

