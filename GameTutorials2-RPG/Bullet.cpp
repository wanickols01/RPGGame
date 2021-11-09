#include "stdafx.h"
#include "Bullet.h"
#include "AnimationC.h"
#include "Movement.h"
#include "Hitbox.h"
#include "Attribute.h"
#include "physicsComponent.h"
#include "PhysicsDevice.h"
#include "bulletC.h"


Bullet::Bullet(float x, float y, float velX, float velY, sf::Texture& texture, const unsigned short state)
{


	GAME_PHYSICS physics(GAME_BODY_TYPE::GAME_DYNAMIC, GAME_OBJECT_SHAPE::GAME_CIRCLE,12,12, 1.f, 1.0f, 1.f, .1f, .7f, 4.f, 0.f, 10.f, true);
	physics.offSetX = 8;
	physics.offSetY = 48;

	std::shared_ptr<physicsComponent> physicsC = std::make_shared<physicsComponent>(physics, *this); //itemComp for player set here

	addComponent(physicsC);


	//Hitbox
	std::shared_ptr<Hitbox> hitbox = std::make_shared<Hitbox>(sprite, 4.f, 2.f, 20.f, 20.f, *this); //hitbox for player set here
	addComponent(hitbox);

	//Animation
	std::shared_ptr<AnimationC> animation = std::make_shared<AnimationC>(sprite, texture, x, y, *this);
	addComponent(animation);
	getComponent<AnimationC>()->addAnimation("ATTACK", 20.f, 0, 0, 7, 0, 32, 32);

	//Movement
	std::shared_ptr<Movement> movement = std::make_shared<Movement>(sprite, 150.f, 450.f, 100.f, *this); //speed for bullet set here
	addComponent(movement);

	std::shared_ptr<Attribute> attribute = std::make_shared<Attribute>(1, *this); //speed for bullet set here
	addComponent(attribute);
	attribute->range = 40;

	std::shared_ptr<bulletC> bullet = std::make_shared<bulletC>(x - physics.offSetX, y - physics.offSetY -28, velX, velY, texture, state, *this); //speed for bullet set here
	addComponent(bullet);

}

void Bullet::update(const float& dt, const sf::Vector2f mousePosView)
{
	Entity::update(dt, mousePosView);
}

void Bullet::render(sf::RenderTarget& target, sf::Shader* shader, sf::Vector2f light_position, const bool show_hitbox)
{
	Entity::render(target, shader, light_position, false);
}
