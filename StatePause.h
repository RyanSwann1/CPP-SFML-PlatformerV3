#pragma once
#include "StateBase.h"

class StatePause :
	public StateBase
{
public:
	StatePause(StateManager* stateManager);
	~StatePause();

	void onEnter() override;
	void onExit() override;

	void onCreate() override;
	void onDestroy() override;

	void update(const float deltaTime);
	void draw(sf::RenderWindow& window);
};

