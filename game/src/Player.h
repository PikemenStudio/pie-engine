//
// Created by anton on 5/14/24.
//

#ifndef ENGINE_PLAYER_H
#define ENGINE_PLAYER_H

#include <SFML/Graphics.hpp>

#include "Animation.h"
#include "Drawable.h"
#include "Keyboard.h"
#include "LightSource.h"
#include "SolidObject.h"
#include "WorldWindow.h"
#include "utils.h"

class Tunnel;
class Rat;

class Player : public SolidObject, public Drawable
{
public:
  Player(sf::Vector2f Center, sf::Vector2f Size, LightSource* Src, Tunnel* CurrTunnel);

  void update(const KeyboardMap& Keyboard, float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects);

  bool isCollision(const SolidObject* Other) const override;
  bool isCollisionWithRat(const Rat* RatObj) const;

  const LightSource* getLightSource() const { return LightSrc; }

  void draw(sf::RenderTarget &Win, const WorldWindow& WorldWindowObj) override;

  sf::Vector2f getPosition() const { return Center; }
  sf::Vector2f getSize() const { return Size; }

  void moveToTunnel(Tunnel* To);

  void setCurrTunnel(Tunnel* T) { CurrTunnel = T; }
  Tunnel* getCurrTunnel() const { return CurrTunnel; }

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

  float getHealth() const { return Health; }
  float getStamina() const { return Stamina; }
  float getOil() const { return Oil; }

private:
  void move(const std::vector<SolidObject*>& Objects, float FrameDrawingTimeS);

  sf::Vector2f Center;
  sf::Vector2f Size;

  // speed is measured in units per second
  sf::Vector2f DxDy = {0, 0};

  float Health = 1.0f, Stamina = 1.0f, Oil = 1.0f;

  bool OnGround = false;
  bool TurnedLeft = false;

  LightSource* LightSrc = nullptr;

  std::unique_ptr<Animation> RunAnim;
  std::unique_ptr<Animation> IdleAnim;
  Animation* CurrAnim = nullptr;

  Tunnel* CurrTunnel;

  sf::Sprite Sprite;

  static constexpr int CharacterHeightPx = 40;
  static constexpr int CharacterCenterXPx = 25;
  static constexpr int CharacterCenterYPx = 28;
};

#endif // ENGINE_PLAYER_H
