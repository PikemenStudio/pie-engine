//
// Created by anton on 5/14/24.
//

#include "Player.h"
#include "Tunnel.h"

#include <loguru.hpp>

void Player::update(const KeyboardMap& Keyboard, float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects)
{
  float FrameDrawingTimeS = FrameDrawingTimeMs / 1000;

  DxDy.x = 0;
  if (Keyboard.at(KeyCode::Left))  DxDy.x -= 0.5;
  if (Keyboard.at(KeyCode::Right)) DxDy.x += 0.5;

  if (Keyboard.at(KeyCode::Up) && OnGround)
  {
    DxDy.y = 1.5;
  }

  DxDy.y -= 6 * FrameDrawingTimeS;
  OnGround = false;

  move(Objects, FrameDrawingTimeS);
}

void Player::move(const std::vector<SolidObject*>& Objects, float FrameDrawingTimeS)
{
  setPosition(Center + sf::Vector2f(DxDy.x * FrameDrawingTimeS, 0));
  if (checkCollisionWithObjects(Objects))
    setPosition(Center - sf::Vector2f(DxDy.x * FrameDrawingTimeS, 0));

  setPosition(Center + sf::Vector2f(0, DxDy.y * FrameDrawingTimeS));
  if (checkCollisionWithObjects(Objects))
  {
    setPosition(Center - sf::Vector2f(0, DxDy.y * FrameDrawingTimeS));

    if (DxDy.y < 0) // we were falling
    {
      DxDy.y = 0;
      OnGround = true;
    }
    else if (!OnGround)
    {
      DxDy.y = 0;
    }
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
