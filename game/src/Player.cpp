//
// Created by anton on 5/14/24.
//

#include "Player.h"
#include "Tunnel.h"

void Player::update(const KeyboardMap& Keyboard, float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects)
{
    float PlayerFrameSpeed = FrameDrawingTimeMs / 1000 * 0.6;

    sf::Vector2f DxDy;
    if (Keyboard.at(KeyCode::Left))  DxDy.x -= PlayerFrameSpeed;
    if (Keyboard.at(KeyCode::Right)) DxDy.x += PlayerFrameSpeed;
    if (Keyboard.at(KeyCode::Up)) DxDy.y += PlayerFrameSpeed;
    if (Keyboard.at(KeyCode::Down)) DxDy.y -= PlayerFrameSpeed;

    move(DxDy, Objects);
}

bool Player::isCollision(const SolidObject* Other) const
{
  if (dynamic_cast<const Player*>(Other))
    return false;
  if (const Tunnel* T = dynamic_cast<const Tunnel*>(Other))
    return T->isCollisionWithPlayer(this);

  return false;
}
