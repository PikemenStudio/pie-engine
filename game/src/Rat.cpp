//
// Created by anton on 5/21/24.
//

#include "Rat.h"
#include "Player.h"
#include "Tunnel.h"

#include "utils.h"

Rat::Rat(const sf::Vector2f& Pos) : Position(Pos)
{
  Size = {0.1f, 0.05f};
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
    return P->isCollisionWithRat(this);
  if (const Tunnel* T = dynamic_cast<const Tunnel*>(Other))
    return T->isCollisionWithBoundingBox(Position, Size);

  return false;
}
