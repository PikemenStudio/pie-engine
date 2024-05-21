//
// Created by anton on 5/21/24.
//

#include "Rat.h"
#include "Player.h"
#include "Tunnel.h"

#include "utils.h"

Rat::Rat(const sf::Vector2f& Pos) : Position(Pos)
{
  Size = {0.1f, 0.065f};
  DxDy = {0, 0};
}

void Rat::update(float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects)
{
  float FrameDrawingTimeS = FrameDrawingTimeMs / 1000;

  DxDy.x = 0.3;
  DxDy.y -= 6 * FrameDrawingTimeS;
  OnGround = false;

  move(Objects, FrameDrawingTimeS);
}

void Rat::move(const std::vector<SolidObject*>& Objects, float FrameDrawingTimeS)
{
  auto OldPos = Position;
  setPosition(Position + sf::Vector2f(DxDy.x * FrameDrawingTimeS, 0));
  if (auto* CollObj = checkCollisionWithObjects(Objects))
  {
    auto PosBeforeSpaceSearch = Position;

    // try to go up and down a little
    setPosition(Position + sf::Vector2f(0, 2 * DxDy.x * FrameDrawingTimeS));

    if (CollObj->isCollision(this))
    {
      setPosition(PosBeforeSpaceSearch);
      setPosition(Position - sf::Vector2f(0, 2 * DxDy.x * FrameDrawingTimeS));

      if (CollObj->isCollision(this))
        setPosition(OldPos);
    }
  }

  setPosition(Position + sf::Vector2f(0, DxDy.y * FrameDrawingTimeS));
  if (checkCollisionWithObjects(Objects))
  {
    setPosition(Position - sf::Vector2f(0, DxDy.y * FrameDrawingTimeS));

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

void Rat::draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj)
{
  sf::RectangleShape Rect(worldDeltaToScreen(Size, WorldWindowObj));

  auto CenterOnScreen = worldCoordsToScreen(Position, WorldWindowObj);
  Rect.setPosition(CenterOnScreen - sf::Vector2f(Rect.getSize().x / 2, Rect.getSize().y / 2));
  Rect.setFillColor(sf::Color::Black);

  Win.draw(Rect);
}

bool Rat::isCollision(const SolidObject* Other) const
{
  if (dynamic_cast<const Rat*>(Other))
    return false;
  if (const Player* P = dynamic_cast<const Player*>(Other))
    return false;
//    return P->isCollisionWithRat(this);
  if (const Tunnel* T = dynamic_cast<const Tunnel*>(Other))
    return T->isCollisionWithBoundingBox(Position, Size);

  return false;
}
