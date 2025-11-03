#include "Player.h"
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cmath>

Player::Player()
{
}

bool Player::init()
{
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("player.png");
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    m_origin = sf::Vector2f(300, 350);
    m_rotation = sf::degrees(45);
    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(3.0f, 3.0f));
    m_collisionRadius = collisionRadius;

    return true;
}

void Player::update(float dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)
        && !m_isJumping)
    {
        if (m_position.y <= 360 
            && m_position.y >= 340
            && m_position.x < 1024)
            m_isJumping = true;
        else 
            m_isJumping = false;
    }

    if (m_isJumping)
    {
        if (m_position.y < 100)
        {
            m_isJumping = false;
            return;
        }
        m_position.y -= 800 * dt;
        //m_position.x += 200 * dt;
    }
    else
    {
        if (m_position.y < 350)
        {
            m_position.y += 800 * dt;
        }
        // if (m_position.x < m_origin.x)
        //     m_position.x -= 200 *dt;
    }
}



void Player::render(sf::RenderTarget& target) const
{
    m_pSprite->setRotation(m_rotation);
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}
