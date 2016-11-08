#pragma once

#include "EntityType.h"
#include "Direction.h"
#include "SpriteSheet.h"
#include "AudioPlayer.h"
#include "CollisionManager.h"
#include <unordered_map>
#include <vector>
#include <SFML\Graphics.hpp>
#include <string>

//afronym: and jump is still on s instead of w
//	afronym : background track stopped playing when i died
//	afronym : probably should add a pause menu or some way to quit
//	afronym : from inside the level
//	afronym : quit button doesn't seem to work
//	afronym : closing the sfml window doesn't actually quit the game (console stays open & music still plays)
//
//
struct Tile;
struct TileInfo;
struct SharedContext;

class Entity
{
	friend class EntityManager;
	friend class CollisionManager;
public:
	Entity(SharedContext* sharedContext);
	virtual ~Entity();
	
	inline void setVelocity(const float x, const float y) { m_velocity = sf::Vector2f(x, y); }

	void setAcceleration(const float x, const float y) { m_acceleration = sf::Vector2f(x, y); }
	void setPosition(const sf::Vector2f& pos) { m_position = pos; }

	EntityType getType() const { return m_type; }
	sf::Vector2i getPosition() const;
	sf::Vector2f getActualPosition() const { return m_position; } //Hacky, needs real name / Maybe change of how i get positions
	std::string getName() const { return m_name; }
	int getID() const { return m_ID; }
	sf::FloatRect getAABB() const { return m_AABB; }
	Direction getDirection() const { return m_currentDirection; }

	void addVelocity(const float x, const float y);
	void move(const float x, const float y);
	void accelerate(const float x, const float y) { m_acceleration += sf::Vector2f(x, y); }
	void applyFriction(const float deltaTime);
	void applyGravity();
	void updateAABB();
	void stop() { m_velocity.x = 0; }

	virtual void update(const float deltaTime);
	virtual void onEntityCollision(Entity* entity) = 0;
	void draw(sf::RenderWindow& window);

protected:
	sf::Vector2f m_position;
	sf::Vector2f m_oldPosition;
	sf::Vector2f m_speed;
	sf::Vector2f m_acceleration;
	sf::Vector2f m_velocity;
	sf::Vector2f m_maxVelocity;
	sf::Vector2f m_friction;
	sf::Vector2f m_minFrictionValue;
	sf::Vector2f m_gravity;
	sf::FloatRect m_AABB;
	std::string m_name;

	Direction m_currentDirection;
	EntityType m_type;
	SpriteSheet m_spriteSheet;
	SharedContext* m_sharedContext;
	AudioPlayer m_audioPlayer;
	CollisionManager m_collisionManager;

	virtual void resolveCollisions() = 0;
	virtual void load(const std::string& fileName) {}

private:
	inline void setID(const int ID)
	{
		if (ID > 0) {
			m_ID = ID;
		}
	}

	int m_ID;
};

