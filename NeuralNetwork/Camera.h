#pragma once

#include "math.h"

class Camera
{
	float zoomVal;
	math::vec3 pos;
	math::mat4 orthoMat;
	math::vec3 viewPortSize;

	friend class CameraComponent;

public:
	Camera();
	~Camera();

	void SetPosition(const math::vec3& _pos);

	math::vec3 const& GetPosition() const;
	math::vec3 const& GetViewPortSize() const;
	float GetZoom()const;

	void SetOrtho(float l, float r, float b, float t, float n, float f);
	math::mat4 Get2DProjection() const;

	void SetZoom(float zoom);
};

