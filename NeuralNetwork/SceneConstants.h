#pragma once

#define BULLET_HEIGHT		SceneConstants::Bullet_Height
#define BULLET_WIDTH		SceneConstants::Bullet_Width
#define BULLET_MIN_SPEED	SceneConstants::Bullet_Min_Speed
#define BULLET_MAX_SPEED	SceneConstants::Bullet_Max_Speed
#define PLAYER_JUMP_FORCE	SceneConstants::Player_Jump_Force
#define DISCRETE_DT			SceneConstants::Discret_Dt

struct SceneConstants
{
public:
	static const float	Bullet_Height;
	static const float	Bullet_Width;
	static const float	Bullet_Min_Speed;
	static const float	Bullet_Max_Speed;
	static const float	Player_Jump_Force;
	static const double	Discret_Dt;
};