#include "Player.h"
#include "ResourceManager.h"
#include "Constants.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>
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

	// Initialize jump members
	m_isJumping = false;
	m_onPlatform = false;
	m_jumpSpeed = 0.0f;
	m_jumpCeiling = 0.0f;
	m_jumpDx = 0.0f;
	m_prevSpaceDown = false;

	return true;
}

void Player::update(float dt)
{
	bool spaceDown = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

	if (spaceDown && !m_prevSpaceDown)
	{
		onJumpPressed();
	}
	m_prevSpaceDown = spaceDown;

	if (m_isJumping)
	{
		m_position.y -= m_jumpSpeed * dt;
		m_position.x += m_jumpDx * dt;

		if (m_position.y <= m_jumpCeiling)
		{
			m_isJumping = false;
			m_rotation = sf::degrees(45);
			return;
		}
	}
	else
	{
		// Calculate new landing position using delta X
		float landingYpos = m_position.x + (m_origin.y - m_origin.x);

		if (m_position.y < landingYpos) // Keep falling
		{
			m_position.y += m_jumpSpeed * dt;
		}
		else // Land
		{
			if (m_origin.x < m_position.x) // Slide back
			{
				m_position.x -= 400 * dt;
				m_position.y -= 400 * dt;
			}
			else // Reset original position
			{
				m_position.x = m_origin.x;
				m_position.y = m_origin.y;
			}
		}
	}
}

void Player::onJumpPressed()
{
	if (m_isJumping)
		return;

	if (m_position.y <= 360 
		&& m_position.y >= 340
		&& m_position.x < 1024)
	{
		if (m_onPlatform)
		{
			m_jumpSpeed = 1000;
			m_jumpCeiling = 50;
			m_onPlatform = false;
			m_jumpDx = 400;
		}
		else
		{
			m_jumpSpeed = 800;
			m_jumpCeiling = 100;
			m_jumpDx = 0;
		}
		m_rotation = sf::degrees(0);
		m_isJumping = true;
	}
	else
		m_isJumping = false;

}

bool Player::checkRectCollision(const sf::RectangleShape& rect)
{
	sf::Vector2f rectCenter = rect.getGlobalBounds().getCenter();

	float distance = (this->m_position - rectCenter).lengthSquared();
	float collisionDist = std::pow(Player::collisionRadius + PlatformHeight / 2, 2.0f);
	
	if (distance <= collisionDist)
	{
		printf("on platform\n");
		return true;
	}

	return false;
}

void Player::setPlatformStatus(bool status)
{
	m_onPlatform = status;
}

bool Player::onPlatform()
{
	return m_onPlatform;
}

void Player::render(sf::RenderTarget& target) const
{
	m_pSprite->setRotation(m_rotation);
	m_pSprite->setPosition(m_position);
	target.draw(*m_pSprite);
}

