//
// Created by anton on 5/14/24.
//

#ifndef ENGINE_PLAYER_H
#define ENGINE_PLAYER_H

#include <SFML/Graphics.hpp>

#include "WorldWindow.h"
#include "Keyboard.h"
#include "LightSource.h"
#include "SolidObject.h"
#include "utils.h"

class Player : public SolidObject
{
public:
  Player(sf::Vector2f Center, sf::Vector2f Size, LightSource* Src)
  {
    this->Center = Center;
    this->Size = Size;
    LightSrc = Src;

    // Sprite
    Tex.loadFromFile("../../game/resources/Run_01.png");
    Tex.setSmooth(false);
    Sprite.setTexture(Tex);

//    Sprite.setScale(2, 2);
  }

  void update(const KeyboardMap& Keyboard, float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects);

  bool isCollision(const SolidObject* Other) const override;

  void draw(sf::RenderTarget &Win, const WorldWindow& WorldWindowObj)
  {
    sf::Vector2f WorldTopLeft = Center + sf::Vector2f(-Size.x / 2, Size.y / 2);
    sf::Vector2f WorldBottomRight = Center + sf::Vector2f(Size.x / 2, -Size.y / 2);

    sf::Vector2f ScreenTopLeft = worldCoordsToScreen(WorldTopLeft, WorldWindowObj);
    sf::Vector2f ScreenBottomRight = worldCoordsToScreen(WorldBottomRight, WorldWindowObj);

    sf::Vector2f CenterScreenCoords = worldCoordsToScreen(Center, WorldWindowObj);

    sf::Vector2f ScreenBBSize = ScreenBottomRight - ScreenTopLeft;
    float SpriteScale = ScreenBBSize.y / CharacterHeightPx;
    Sprite.setScale(SpriteScale, SpriteScale);
    Sprite.setPosition(CenterScreenCoords -
                       sf::Vector2f(CharacterCenterXPx * SpriteScale,
                                    CharacterCenterYPx * SpriteScale));

    if (DxDy.x < 0 || (TurnedLeft && DxDy.x == 0))
    {
      TurnedLeft = true;
      Sprite.scale(-1, 1);
      Sprite.move(CharacterCenterXPx * 2 * SpriteScale, 0);
    }
    else if (DxDy.x > 0 || (!TurnedLeft && DxDy.x == 0))
    {
      TurnedLeft = false;
      Sprite.scale(1, 1);
    }

    sf::RectangleShape Rect(ScreenBottomRight - ScreenTopLeft);
    Rect.setPosition(ScreenTopLeft);
    Rect.setOutlineThickness(5);
    Rect.setOutlineColor(sf::Color::Cyan);

    Win.draw(Rect);
    Win.draw(Sprite);
  }

  sf::Vector2f getPosition() const { return Center; }
  sf::Vector2f getSize() const { return Size; }

  void setPosition(sf::Vector2f C)
  {
    this->Center = C;

    if (LightSrc)
      LightSrc->setPosition(C);
  }

  void setPositionWithCollisionCheck(sf::Vector2f Pos,
                                     const std::vector<SolidObject*>& Objects)
  {
    auto OldPos = Center;
    setPosition(Pos);

    for (auto* Obj : Objects)
    {
      if (isCollision(Obj))
      {
        setPosition(OldPos);
        break;
      }
    }
  }

private:
  void move(const std::vector<SolidObject*>& Objects, float FrameDrawingTimeS);

  sf::Vector2f Center;
  sf::Vector2f Size;

  // speed is measured in units per second
  sf::Vector2f DxDy = {0, 0};

  bool OnGround = false;
  bool TurnedLeft = false;

  LightSource* LightSrc = nullptr;

  sf::Texture Tex;
  sf::Sprite Sprite;

  static constexpr int CharacterHeightPx = 40;
  static constexpr int CharacterCenterXPx = 25;
  static constexpr int CharacterCenterYPx = 28;
};

#endif // ENGINE_PLAYER_H
