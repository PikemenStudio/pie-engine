//
// Created by anton on 5/14/24.
//

#include "Player.h"
#include "Tunnel.h"

#include <loguru.hpp>

void Player::update(const KeyboardMap& Keyboard, float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects)
{
  float FrameSpeed = FrameDrawingTimeMs / 1000 * 0.6;

  DxDy.x = 0;
  if (Keyboard.at(KeyCode::Left))  DxDy.x -= FrameSpeed;
  if (Keyboard.at(KeyCode::Right)) DxDy.x += FrameSpeed;

  if (Keyboard.at(KeyCode::Up)) DxDy.y = FrameSpeed; // for now the player can fly
//  if (Keyboard.at(KeyCode::Down)) DxDy.y -= FrameSpeed;

  DxDy.y += -FrameSpeed / 10; // gravity

  move(Objects);
}

void Player::move(const std::vector<SolidObject*>& Objects)
{
  LOG_F(INFO, "DxDy.y == %f", DxDy.y);

  setPosition(Center + sf::Vector2f(DxDy.x, 0));
  if (checkCollisionWithObjects(Objects))
    setPosition(Center - sf::Vector2f(DxDy.x, 0));

  setPosition(Center + sf::Vector2f(0, DxDy.y));
  if (checkCollisionWithObjects(Objects))
  {
    setPosition(Center - sf::Vector2f(0, DxDy.y));

    if (DxDy.y < 0) // we were falling
      DxDy.y = 0;
  }
}

bool Player::isCollision(const SolidObject* Other) const
{
  if (dynamic_cast<const Player*>(Other))
    return false;
  if (const Tunnel* T = dynamic_cast<const Tunnel*>(Other))
    return T->isCollisionWithPlayer(this);

  return false;
}
