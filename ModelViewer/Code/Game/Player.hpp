#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"

class Player: public Entity
{
public:
	Player(Game* owner);
	virtual ~Player();
	
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;

	Camera* GetCamera();
public:
private:
	Camera* m_playerCamera = nullptr;
	float m_movingSpeed = 4.f;
	float m_rotatingSpeed = 90.f;
private:
	void HandleInput(float deltaSeconds);
	void Movement(float deltaSeconds);
};