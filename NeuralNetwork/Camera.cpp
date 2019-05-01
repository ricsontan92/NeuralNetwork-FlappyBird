#include "Camera.h"

Camera::Camera() : orthoMat(math::mat4::GetIdentity()), pos(0.f, 0.f, 0.f), zoomVal(1.f)
{
}

void Camera::SetOrtho(float l, float r, float b, float t, float n, float f)
{
	viewPortSize.x = fabs(r - l);
	viewPortSize.y = fabs(t - b);

	orthoMat = math::mat4::Ortho(l, r, b, t, n, f);
}

void Camera::SetPosition(const math::vec3& _pos)
{
	pos = _pos;
}

void Camera::SetZoom(float zoom)
{
	zoomVal = Clamp(zoom, 0.5f, 3.f);
}

math::vec3 const& Camera::GetPosition() const
{
	return pos;
}

math::vec3 const& Camera::GetViewPortSize() const
{
	return viewPortSize;
}

math::mat4 Camera::Get2DProjection() const
{
	return orthoMat * math::mat4::LookAt(math::vec3(pos.x, pos.y, 0.f), math::vec3(pos.x, pos.y, 1.f), math::vec3(0.f, 1.f, 0.f)) * math::mat4::Scale(zoomVal);
}

float Camera::GetZoom()const
{
	return zoomVal;
}

Camera::~Camera()
{
}
