#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>


namespace sf { class Sprite; }

class Player final : public Entity
{
public:
    static constexpr float collisionRadius = 42.0f;

    Player();
    virtual ~Player() = default;
    
    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;
    void onJumpPressed();
    void resetJump();

    bool checkRectCollision(const sf::RectangleShape& rect);
    void setPlatformStatus(bool status);
    bool onPlatform();
    
    protected:
    sf::Vector2f m_origin;

    // jump state
    bool m_onPlatform;
    bool m_isJumping = false;
    bool m_prevSpaceDown = false;
    float m_jumpSpeed = 0.0f;
    float m_jumpCeiling = 0.0f;
    float m_jumpDx = 0.0f;
};
