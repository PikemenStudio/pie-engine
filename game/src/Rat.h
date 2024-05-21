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

  void update(float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects);

  void setPosition(sf::Vector2f Pos) { Position = Pos; }
  void setSize(sf::Vector2f S) { Size = S; }

  sf::Vector2f getPosition() const { return Position; }
  sf::Vector2f getSize() const { return Size; }

  void draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj) override;
  bool isCollision(const SolidObject* Other) const override;

private:
  void move(const std::vector<SolidObject*>& Objects, float FrameDrawingTimeS);

  sf::Vector2f Position;
  sf::Vector2f Size;
  sf::Vector2f DxDy;

  bool OnGround;
};

#endif // ENGINE_RAT_H
