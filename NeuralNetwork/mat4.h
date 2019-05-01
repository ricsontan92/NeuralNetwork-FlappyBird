#pragma once

namespace math
{
	class vec3;

	class mat4
	{
	public:
		union
		{
			float m16[16];
			float m44[4][4];
		};

		mat4();
		~mat4();

		void Transpose();
		mat4 GetTranspose() const;
		mat4 operator*(float rhs) const;
		mat4 operator*(const mat4& rhs) const;
		vec3 operator*(const vec3& rhs) const;
		void GetAxisAngle(float& angle, math::vec3& axis) const;

		static mat4 GetIdentity();
		static mat4 Translate(const math::vec3& rhs);
		static mat4 Scale(const math::vec3& rhs);
		static mat4 Scale(float rhs);
		static mat4 Rotate(float deg, const math::vec3& axis);
		static mat4 Rotate2D(float deg);

		static mat4 Perspective(float deg, float aspect, float nr, float fr);
		static mat4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
		static mat4 LookAt(const math::vec3& pos, const math::vec3& target, const math::vec3& up);
	};
}