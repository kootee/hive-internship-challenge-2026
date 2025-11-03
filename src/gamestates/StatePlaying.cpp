#include "StatePlaying.h"
#include "StatePaused.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <memory>
#include <iostream>
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cstdlib>
#include <algorithm>
#include <random>

const float PlatformHeight = 24.0f;
const float PlatformWidth = 120.0f;

StatePlaying::StatePlaying(StateStack& stateStack)
	: m_stateStack(stateStack)
{
}

bool StatePlaying::init()
{
	m_ground.setSize({1448.2f, 1024.0f});
	m_ground.setPosition({-50.0f, 0.0f});
	m_ground.setFillColor(sf::Color::White);
	m_ground.setRotation(sf::degrees(45));

	m_pPlayer = std::make_unique<Player>();
	if (!m_pPlayer || !m_pPlayer->init())
		return false;
	m_pPlayer->setPosition(sf::Vector2f(300, 350));

	return true;
}

void StatePlaying::update(float dt)
{
	m_timeUntilEnemySpawn -= dt;

	// Enemy spawn
	if (m_timeUntilEnemySpawn < 0.0f)
	{
		std::random_device rd;
    	std::mt19937 gen(rd());
    	std::uniform_real_distribution<float> dis(0.5f, 3.0f);

    	float randomInterval = dis(gen);
		
		m_timeUntilEnemySpawn = randomInterval;
		std::unique_ptr<Enemy> pEnemy = std::make_unique<Enemy>();
		pEnemy->setPosition(sf::Vector2f(974, 1024));
		if (pEnemy->init())
			m_enemies.push_back(std::move(pEnemy));
	}

	// Platform spawn
	m_timeUntilPlatformSpawn -= dt;
	if (m_timeUntilPlatformSpawn <= 0.0f)
	{
		spawnPlatform();
		m_timeUntilPlatformSpawn = platformSpawnInterval;
	}
	
	// Pause
	bool isPauseKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
	m_hasPauseKeyBeenReleased |= !isPauseKeyPressed;
	if (m_hasPauseKeyBeenReleased && isPauseKeyPressed)
	{
		m_hasPauseKeyBeenReleased = false;
		m_stateStack.push<StatePaused>();
	}

	// Player
	m_pPlayer->setPlatformStatus(false);
	for (auto it : m_platforms)
	{
		if (m_pPlayer->checkRectCollision(it))
		{
			m_pPlayer->setPlatformStatus(true);
			break;
		}
	}
	m_pPlayer->update(dt);
	updatePlatforms(dt);

	for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
	{
		pEnemy->update(dt);
	}

	// Detect collisions
	bool playerDied = false;
	for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
	{
		float distance = (m_pPlayer->getPosition() - pEnemy->getPosition()).lengthSquared();
		float minDistance = std::pow(Player::collisionRadius + pEnemy->getCollisionRadius(), 2.0f);

		if (distance <= minDistance)
		{
			playerDied = true;
			break;
		}
	}

	if (playerDied)
		m_stateStack.popDeferred();
}

void StatePlaying::render(sf::RenderTarget& target) const
{
	target.draw(m_ground);

	for (const sf::RectangleShape& platform : m_platforms)
		target.draw(platform);
	for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
		pEnemy->render(target);
	m_pPlayer->render(target);
}

void StatePlaying::spawnPlatform()
{
	const float width = PlatformWidth;
	const float height = PlatformHeight;
	sf::RectangleShape platform({width, height});
	const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("bark.png");
	platform.setTexture(pTexture);
	if (pTexture == nullptr)
	{
		platform.setFillColor(sf::Color(150, 75, 0));
	}
	platform.setPosition(sf::Vector2f(974, 1024));
	m_platforms.push_back(std::move(platform));
}

void StatePlaying::updatePlatforms(float dt)
{
	if (m_platforms.empty())
		return;

	float dx = 200 * dt;
	float dy = 200 * dt;

	for (auto& platform : m_platforms)
		platform.move(sf::Vector2f(-dx, -dy));

	for (auto it = m_platforms.begin(); it != m_platforms.end();)
	{
		if ((it->getPosition().x + it->getSize().x) < -50.0f)
			it = m_platforms.erase(it);
		else
			++it;
	}
}
