#include "StatePause.h"
#include "GUIManager.h"
#include "SharedContext.h"
#include "StateManager.h"
#include "FontManager.h"

StatePause::StatePause(StateManager* stateManager)
	: StateBase(stateManager)
{
}


StatePause::~StatePause()
{
}

void StatePause::onEnter()
{
		
}

void StatePause::onExit()
{
}

void StatePause::onCreate()
{
	GUIManager* guiManager = m_stateManager->getSharedContext()->m_guiManager;
	FontManager* fontManager = m_stateManager->getSharedContext()->m_fontManager;
	
	GUIInterface pausedInterface("Paused");
	pausedInterface.constructNewGuiElement(fontManager, GuiStyle(sf::Color::White, 24, sf::Vector2f(50, 150), "Magnificient"), "Paused", sf::Vector2f(400, 400));
	guiManager->addInterface("Paused", pausedInterface);
}

void StatePause::onDestroy()
{
	GUIManager* guiManager = m_stateManager->getSharedContext()->m_guiManager;
	guiManager->removeInterface("Paused");
}

void StatePause::update(const float deltaTime)
{
}

void StatePause::draw(sf::RenderWindow & window)
{
	GUIManager* guiManager = m_stateManager->getSharedContext()->m_guiManager;
	guiManager->draw(window);
}
