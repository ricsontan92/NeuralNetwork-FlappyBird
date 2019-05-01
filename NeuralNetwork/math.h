#pragma once

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "quat.h"
#include "mat4.h"

static const float PIf = 3.1415926535897932384626433832795028841971693993751058209749445923078164062f;
static const double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062;

static const float __RD__ = 180.f / PIf;	// radians to degree
static const float __DR__ = PIf / 180.f;	// degree to radians

#define RAD_TO_DEG(x) (x * __RD__)
#define DEG_TO_RAD(x) (x * __DR__)

template<typename T>
T Clamp(const T& val, const T& min, const T& max)
{
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

template<typename T, typename F>
T Interpolate(const T& start, const T& end, const F& time)
{
	return start + (end - start) * time;
}