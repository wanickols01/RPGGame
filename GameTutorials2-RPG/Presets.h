#pragma once
#include "stdafx.h"
#include "Constants.h"

struct hitBoxPreset {
	bool created = false;
	float width = 0.f;
	float height = 0.f;
	float offSetX = 0.f;
	float offSetY = 0.f;

};

struct movementPreset 
{
	bool created = false;
	sf::Vector2f maxVelocity;
	sf::Vector2f acceleration;
	sf::Vector2f deceleration;
};


struct attributePreset 
{
	bool created = false;
	int level;
	int range;
};
struct enemyDataPreset 
{
	bool created = false;
	std::string enemyName = "";
	float expMult;
	int enemyPowerLevel;
	float vitalityMult;
	float strengthMult;
	float dexterityMult;
	float agilityMult;
	float intellegenceMult;
	int xp;
};


struct AnimationPreset //individual animations
{
	std::string key = "";
	float animation_timer = 0.f;  
	int start_frame_x = 0;  
	int start_frame_y = 0;
	int frames_x; int frames_y = 0;
	int width = 0;
	int height = 0;
	
};

struct AnimationCPreset 
{
	bool created = false;
	int numAnimations = 0;
	std::vector<std::shared_ptr<AnimationPreset>> animations;
	float x;
	float y;
};

struct AIPreset 
{
	bool created = false;
	bool follow = false;
};

struct combatPreset 
{
	bool created = false;
};

struct physicsPreset
{
	bool created = false;
	GAME_PHYSICS gamePhysics;
};

struct allEnemyPresets 
{
	hitBoxPreset hitBox;
	movementPreset movement;
	enemyDataPreset enemyData;
	attributePreset attribute;
	AnimationCPreset animation;
	combatPreset combat;
	physicsPreset physics;
	AIPreset ai;
};