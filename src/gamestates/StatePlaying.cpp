#include "StatePlaying.h"
#include "StatePaused.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <memory>
#include <iostream>
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>

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

    m_slope = std::make_unique<Slope>();

    return true;
}

void StatePlaying::update(float dt)
{
    m_timeUntilEnemySpawn -= dt;

    // Enemy spawn
    if (m_timeUntilEnemySpawn < 0.0f)
    {
        m_timeUntilEnemySpawn = enemySpawnInterval;
        std::unique_ptr<Enemy> pEnemy = std::make_unique<Enemy>();
        pEnemy->setPosition(sf::Vector2f(974, 1024));
        if (pEnemy->init())
            m_enemies.push_back(std::move(pEnemy));
    }

    // Pause
    bool isPauseKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
    m_hasPauseKeyBeenReleased |= !isPauseKeyPressed;
    if (m_hasPauseKeyBeenReleased && isPauseKeyPressed)
    {
        m_hasPauseKeyBeenReleased = false;
        m_stateStack.push<StatePaused>();
    }

    // Player and  slope update
    m_pPlayer->update(dt);
    m_slope->update(dt);

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
        //const sf::Vector2f playerPosition = m_pPlayer->getPosition();

        if (distance <= minDistance)
        {
            playerDied = true;
            break;
        }
    }

    // End Playing State on player death
    if (playerDied)
        m_stateStack.popDeferred();
}

void StatePlaying::render(sf::RenderTarget& target) const
{
    target.draw(m_ground);
    target.draw(m_slope->getTerrainVertices()); // draw terrain
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
        pEnemy->render(target);
    m_pPlayer->render(target);
}
