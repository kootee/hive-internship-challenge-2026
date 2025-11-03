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

    bool checkRectCollision(const sf::RectangleShape& rect);
    void setPlatformStatus(bool status);
    bool onPlatform();
    
protected:
    sf::Vector2f m_origin;

    // jump state member values
    bool m_onPlatform;
    bool m_isJumping;
    float m_jumpSpeed;
    float m_jumpCeiling;
    float m_jumpDx;
    bool m_prevSpaceDown;
};
