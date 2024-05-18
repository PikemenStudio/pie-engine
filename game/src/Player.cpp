//
// Created by anton on 5/14/24.
//

#include "Player.h"
#include "Tunnel.h"

#include <loguru.hpp>

Player::Player(sf::Vector2f Center, sf::Vector2f Size, LightSource* Src)
{
  this->Center = Center;
  this->Size = Size;
  LightSrc = Src;

  RunAnim = std::make_unique<Animation>(std::vector<std::string> {
      "../../game/resources/CartoonDetective/Run/Run_01.png",
      "../../game/resources/CartoonDetective/Run/Run_02.png",
      "../../game/resources/CartoonDetective/Run/Run_03.png",
      "../../game/resources/CartoonDetective/Run/Run_04.png",
      "../../game/resources/CartoonDetective/Run/Run_05.png",
      "../../game/resources/CartoonDetective/Run/Run_06.png",
      "../../game/resources/CartoonDetective/Run/Run_07.png",
      "../../game/resources/CartoonDetective/Run/Run_08.png",
  });
  RunAnim->setLoop(true);
  RunAnim->setFrameTime(0.1);

  IdleAnim = std::make_unique<Animation>(std::vector<std::string> {
      "../../game/resources/CartoonDetective/Idle/Idle_01.png",
      "../../game/resources/CartoonDetective/Idle/Idle_02.png",
      "../../game/resources/CartoonDetective/Idle/Idle_03.png",
      "../../game/resources/CartoonDetective/Idle/Idle_04.png",
  });
  IdleAnim->setLoop(true);
  IdleAnim->setFrameTime(0.1);

  CurrAnim = IdleAnim.get();
  CurrAnim->start();
}

void Player::draw(sf::RenderTarget &Win, const WorldWindow& WorldWindowObj)
{
  sf::Vector2f WorldTopLeft = Center + sf::Vector2f(-Size.x / 2, Size.y / 2);
  sf::Vector2f WorldBottomRight = Center + sf::Vector2f(Size.x / 2, -Size.y / 2);

  sf::Vector2f ScreenTopLeft = worldCoordsToScreen(WorldTopLeft, WorldWindowObj);
  sf::Vector2f ScreenBottomRight = worldCoordsToScreen(WorldBottomRight, WorldWindowObj);

  sf::Vector2f CenterScreenCoords = worldCoordsToScreen(Center, WorldWindowObj);

  sf::Vector2f ScreenBBSize = ScreenBottomRight - ScreenTopLeft;

  constexpr float CharacterHeightToBoundingBox = 1.1;
  float SpriteScale = ScreenBBSize.y / CharacterHeightPx * CharacterHeightToBoundingBox;
  Sprite.setTexture(*CurrAnim->getFrames()[CurrAnim->getCurrFrameIndex()]);
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

  //    Win.draw(Rect); // Draw bounding box
  Win.draw(Sprite);
}

void Player::update(const KeyboardMap& Keyboard, float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects)
{
  CurrAnim->update();

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

  if (DxDy.x != 0 && CurrAnim != RunAnim.get())
  {
    IdleAnim->stop();
    CurrAnim = RunAnim.get();
    CurrAnim->start();
  }
  else if (DxDy.x == 0 && CurrAnim != IdleAnim.get())
  {
    RunAnim->stop();
    CurrAnim = IdleAnim.get();
    CurrAnim->start();
  }

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
