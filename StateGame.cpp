#include "StateGame.h"
#include "SharedContext.h"
#include "StateManager.h"
#include "EntityManager.h"
#include "Map.h"
#include "Player.h"
#include "Rat.h"
#include "Window.h"
#include "Character.h"

StateGame::StateGame(StateManager* stateManager)
	: StateBase(stateManager),
	m_camera(stateManager->getSharedContext()),
	m_map(nullptr),
	m_audioPlayer(stateManager->getSharedContext())
{
	
}

StateGame::~StateGame()
{
}

void StateGame::onCreate()
{
	m_map = new Map(m_stateManager->getSharedContext());
	SharedContext* sharedContext = m_stateManager->getSharedContext();
	m_map->loadNextLevel();
	sharedContext->m_entityManager->add(new Player(sharedContext));

	m_audioPlayer.registerAudioClip("Game", "BackgroundMusic");
	m_audioPlayer.registerOwner("Game");
}

void StateGame::onEnter()
{
	//m_audioPlayer.play("BackgroundMusic", true);
}

void StateGame::onExit()
{
	m_camera.reset();
	m_audioPlayer.stop("BackgroundMusic");
}

void StateGame::onDestroy()
{
	delete m_map;
	m_map = nullptr;

	m_stateManager->getSharedContext()->m_map = nullptr;
}

void StateGame::update(const float deltaTime)
{
	m_map->update();

	SharedContext* sharedContext = m_stateManager->getSharedContext();
	Window* window = sharedContext->m_window;
	//Set position for the state's view
	Character* player = static_cast<Character*>(sharedContext->m_entityManager->findEntity("Player"));
	if (player)
	{
		m_camera.update(player);
	}
	//If player does not exist, spawn new player
	else
	{
		if (player->getLives())
		{
			sharedContext->m_entityManager->add(new Player(sharedContext));
		}
		else
		{

		}
		
		m_camera.reset();
	}
	
	sharedContext->m_entityManager->update(deltaTime);
	
}

void StateGame::draw(sf::RenderWindow & window)
{
	SharedContext* sharedContext = m_stateManager->getSharedContext();
	
	m_map->draw(window);
	sharedContext->m_entityManager->draw(window);
}