//
// Created by anton on 5/14/24.
//

#ifndef ENGINE_PLAYER_H
#define ENGINE_PLAYER_H

#include <SFML/Graphics.hpp>

#include "Animation.h"
#include "Keyboard.h"
#include "LightSource.h"
#include "SolidObject.h"
#include "WorldWindow.h"
#include "utils.h"

class Player : public SolidObject
{
public:
  Player(sf::Vector2f Center, sf::Vector2f Size, LightSource* Src);

  void update(const KeyboardMap& Keyboard, float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects);

  bool isCollision(const SolidObject* Other) const override;

  void draw(sf::RenderTarget &Win, const WorldWindow& WorldWindowObj);

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

  std::unique_ptr<Animation> RunAnim;
  std::unique_ptr<Animation> IdleAnim;
  Animation* CurrAnim = nullptr;

  sf::Sprite Sprite;

  static constexpr int CharacterHeightPx = 40;
  static constexpr int CharacterCenterXPx = 25;
  static constexpr int CharacterCenterYPx = 28;
};

#endif // ENGINE_PLAYER_H
