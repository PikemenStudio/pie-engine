//
// Created by anton on 5/21/24.
//

#ifndef ENGINE_RAT_H
#define ENGINE_RAT_H

#include "Drawable.h"
#include "SolidObject.h"

class Rat : public Drawable, public SolidObject
{
public:
  Rat(const sf::Vector2f& Pos);

  sf::Vector2f getPosition() const { return Position; }
  sf::Vector2f getSize() const { return Size; }

  void draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj) override;
  bool isCollision(const SolidObject* Other) const override;

private:
  sf::Vector2f Position;
  sf::Vector2f Size;
};

#endif // ENGINE_RAT_H
