#include "Character.h"
#include "SharedContext.h"
#include "Map.h"
#include "EntityManager.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>


Character::Character(SharedContext* sharedContext)
	: Entity(sharedContext),
	m_jumping(false),
	m_jumpFinished(true),
	m_jumpTime(0.5f)
{
	m_jumpingTimer.setExpirationTime(m_jumpTime);
}

Character::~Character()
{
}

void Character::load(const std::string & fileName)
{
	std::ifstream file;
	file.open(fileName);
	if (!file.is_open())
	{
		std::cerr << "Unable to open file: " << fileName << "\n";
		return;
	}
	std::string line;
	while (std::getline(file, line))
	{
		std::stringstream keyStream(line);
		std::string type;
		std::string spriteSheetName;

		keyStream >> type;
		if (type == "SpriteSheetDetails")
		{
			keyStream >> spriteSheetName;
			m_spriteSheet.loadIn(spriteSheetName);

			int tileSize = m_spriteSheet.getTileSize();
			m_attackAABB.width = tileSize;
			m_attackAABB.height = tileSize;
		}
		else if (type == "Lives")
		{
			keyStream >> m_lives >> m_maxLives;
		}
		else if (type == "Damage")
		{
			keyStream >> m_damage;
		}
		else if (type == "Speed")
		{
			keyStream >> m_speed.x >> m_speed.y;
		}
		else if (type == "JumpVelocity")
		{
			keyStream >> m_jumpVelocity;
		}
		else if (type == "MaxVelocity")
		{
			keyStream >> m_maxVelocity.x >> m_maxVelocity.y;
		}
		else if (type == "Position")
		{
			keyStream >> m_position.x >> m_position.y;
		}
		else if (type == "JumpTime")
		{
			keyStream >> m_jumpTime;
		}
	}
}

void Character::move(const Direction dir)
{
	if (m_currentState != EntityState::Dead && m_currentState != EntityState::Attacking)
	{
		switch (dir)
		{
		case (Direction::Left):
		{
			setDirection(Direction::Left);
			setVelocity(-m_speed.x, m_velocity.y);
			break;
		}
		case (Direction::Right):
		{
			setDirection(Direction::Right);
			setVelocity(m_speed.x, m_velocity.y);
			break;
		}
		}
	}
}

void Character::update(const float deltaTime)
{
	Entity::update(deltaTime);

	if (m_currentState != EntityState::Attacking && m_currentState != EntityState::Dead)
	{
		if (m_velocity.x < -1.0f || m_velocity.x > 1.0f)
		{
			setState(EntityState::Walking);

		}
		else {
			setState(EntityState::Idle);
		}
	}

	handleJumping(deltaTime);
	handleAnimations();
	m_spriteSheet.setSpritePosition(m_position);
	m_spriteSheet.update(deltaTime);
}

void Character::reduceLife(const int damage)
{
	if (m_currentState == EntityState::Dead) {
		return;
	}

	if ((m_lives - damage) <= 0)
	{
		m_lives = 0;
		m_velocity.x = 0;
		setState(EntityState::Dead);
		m_spriteSheet.setAnimationType("Dead", m_currentDirection);
		
	}
	else {
		m_lives -= damage;
	}
}

void Character::attackPreperation()
{
	if (m_currentState == EntityState::Dead || m_currentState == EntityState::Attacking) {
		return;
	}

	setState(EntityState::Attacking);
	m_spriteSheet.setAnimationType("Attack", m_currentDirection);
	m_audioPlayer.play("Attack", false);
	m_velocity.x = 0;
	int tileSize = m_spriteSheet.getTileSize();
	m_attackAABB.top = m_AABB.top;
	int y = m_attackAABB.top / tileSize;
	int fromX = 0;
	int toX = 0;
	Map* map = m_sharedContext->m_map;

	//Get attack information from specific direction
	switch (m_currentDirection)
	{
	case (Direction::Right) : 
	{
		m_attackAABB.left = m_AABB.left + m_AABB.width;
		fromX = std::floor((m_attackAABB.left) / tileSize);
		toX = std::floor((m_attackAABB.left + (m_attackAABB.width * 2.0f)) / tileSize);
		break;
	}
	case (Direction::Left) :
	{
		m_attackAABB.left = m_AABB.left + (tileSize / 2.0f);

		toX = std::floor(m_attackAABB.left / tileSize);
		fromX = std::floor((m_attackAABB.left - (m_attackAABB.width * 2.0f)) / tileSize);
		break;
	}
	}

	for (int x = fromX; x < toX; ++x)
	{
		//sf::FloatRect rect = entity->getAABB();
		sf::RectangleShape attackRect;
		attackRect.setPosition(x * tileSize, y * tileSize);
		attackRect.setSize(sf::Vector2f(tileSize, tileSize));
		attackRect.setFillColor(sf::Color::Blue);
		//m_attackPositions.push_back(attackRect);

		Entity* entity = m_sharedContext->m_entityManager->getEntityAtPosition(sf::Vector2i(x, y));
		if (entity)
		{
			//Make sure entity doesn't attack itself
			if (entity->getID() != getID())
			{
				attack(entity, x, y);
			}
		}
	}	
}

void Character::jump()
{
	if (m_jumpFinished && m_currentState != EntityState::Dead)
	{
		m_audioPlayer.play("Jump", false);
		m_jumping = true;
		m_jumpFinished = false;
		m_jumpingTimer.activate();
	}
}

void Character::killCharacter()
{
	if (m_currentState != EntityState::Dead)
	{
		setState(EntityState::Dead);
		m_audioPlayer.play("Death", false);
		m_spriteSheet.setAnimationType("Dead", m_currentDirection);
		m_velocity = sf::Vector2f(0, 0);
	}
}

void Character::attack(Entity* entity, const int x, const int y)
{
	int tileSize = m_spriteSheet.getTileSize();
	sf::Vector2i entityPos = entity->getPosition();
	sf::FloatRect attackBox(x * tileSize, y * tileSize, tileSize, tileSize);
	if (attackBox.intersects(entity->getAABB()))
	{
		entity->onEntityCollision(this);
	}
	else
	{
		sf::FloatRect rect = entity->getAABB();
		sf::RectangleShape attackRect;
		attackRect.setPosition(rect.left, rect.top);
		attackRect.setSize(sf::Vector2f(tileSize, tileSize));
		attackRect.setFillColor(sf::Color::Green);
		//m_attackPositions.push_back(attackRect);
	}
}

void Character::handleAnimations()
{
	switch (m_currentState)
	{
	case (EntityState::Idle) :
	{
		//Make the spritesheet/Animation class handle the difference between the direction
		//m_spriteSheet.setAnimationType("Idle", m_currentDirection);
		//m_spriteSheet.setAnimationType(AnimationNames::Names::m_animationNames[static_cast<int>(EntityState::Idle)], m_currentDirection);
		m_spriteSheet.setAnimationType("Idle", m_currentDirection);
		break;
	}
	case (EntityState::Walking) :
	{
		m_spriteSheet.setAnimationType("Walk", m_currentDirection);
		break;
	}
	case (EntityState::Attacking) :
	{
		//Attack animation will not repeat
		if (m_spriteSheet.getCurrentAnimation()->isFinished())
		{
			m_spriteSheet.getCurrentAnimation()->stop();
			if (m_velocity.x <= -0.1f || m_velocity.x >= 0.1f)
			{
				setState(EntityState::Walking);
			}
			else {
				setState(EntityState::Idle);
			}
		}
		break;
	}
	case (EntityState::Dead) :
	{
		if (m_spriteSheet.getCurrentAnimation()->isFinished())
		{
			m_sharedContext->m_entityManager->removeEntity(getID());
		}
		break;
	}
	}
}

void Character::handleJumping(const float deltaTime)
{
	if (m_jumping)
	{
		m_jumpingTimer.update(deltaTime);

		addVelocity(0, -m_jumpVelocity);
		if (m_jumpingTimer.isExpired())
		{
			m_jumping = false;
			m_jumpingTimer.deactivate();
		}
	}
}

void Character::resolveCollisions()
{
	if (m_collisionManager.collidingOnYAxis()) {
		m_jumpFinished = true;
	}

	//If colliding with beginning of map

}