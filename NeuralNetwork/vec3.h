#pragma once

#include <ostream>

namespace math
{
	class vec2;
	class vec3
	{
	public:
		union
		{
			struct { float x, y, z; };
			float data[3];
		};

		vec3(float _x, float _y, float _z);
		vec3();
		~vec3();

		math::vec2 xy() const;
		math::vec2 yz() const;
		math::vec2 xz() const;

		float LenSq() const;
		float Len() const;
		float Dot(const vec3& rhs) const;
		float DistSq(const vec3& rhs) const;
		float Dist(const vec3& rhs) const;
		void Normalize();
		vec3 GetNormalize() const;
		vec3 GetAbsolute() const;
		vec3 Cross(const vec3& rhs) const;

		vec3 operator-()const;
		vec3 operator-(const vec3& rhs)const;
		vec3 operator+(const vec3& rhs)const;
		vec3 operator*(float rhs)const;
		vec3 operator/(float rhs)const;

		vec3 operator/(const vec3& rhs)const;	// component wise
		vec3 operator*(const vec3& rhs)const;

		vec3& operator+=(const vec3& rhs);
		vec3& operator-=(const vec3& rhs);
		vec3& operator/=(float rhs);
		vec3& operator*=(float rhs);

		float& operator[](int index);
		float operator[](int index) const;

		bool operator==(const vec3& rhs) const;
		bool operator!=(const vec3& rhs) const;

		static const vec3& XAxis();
		static const vec3& YAxis();
		static const vec3& ZAxis();

		friend std::ostream& operator<<(std::ostream& os, const math::vec3& val);
	};
}
