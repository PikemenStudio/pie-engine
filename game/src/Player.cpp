//
// Created by anton on 5/14/24.
//

#include "Player.h"
#include "Tunnel.h"

#include <loguru.hpp>

void Player::update(const KeyboardMap& Keyboard, float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects)
{
  RunAnim->update();

  float FrameDrawingTimeS = FrameDrawingTimeMs / 1000;

  DxDy.x = 0;
  if (Keyboard.at(KeyCode::Left))  DxDy.x -= 0.2;
  if (Keyboard.at(KeyCode::Right)) DxDy.x += 0.2;

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
  auto OldPos = Center;
  setPosition(Center + sf::Vector2f(DxDy.x * FrameDrawingTimeS, 0));
  if (auto* CollObj = checkCollisionWithObjects(Objects))
  {
    auto PosBeforeSpaceSearch = Center;

    // try to go up and down a little
    setPosition(Center + sf::Vector2f(0, 2 * DxDy.x * FrameDrawingTimeS));

    if (CollObj->isCollision(this))
    {
      setPosition(PosBeforeSpaceSearch);
      setPosition(Center - sf::Vector2f(0, 2 * DxDy.x * FrameDrawingTimeS));

      if (CollObj->isCollision(this))
        setPosition(OldPos);
    }
  }

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
