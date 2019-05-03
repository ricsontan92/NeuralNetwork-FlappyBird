#pragma once

#define DISCRETE_DT			SceneConstants::Discret_Dt

struct SceneConstants
{
public:
	static const double	Discret_Dt;
	static const float	FlapDelay;
	static const float	FlapStrength;
	static const float	HoleHeight;
	static const float	BirdSize;
	static const float	HoleDistanceRange;
	static const float	ObstacleSpawnTime;
	static const float	ObstacleInitialSpeed;
};