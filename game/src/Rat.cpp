//
// Created by anton on 5/21/24.
//

#include "Rat.h"
#include "Player.h"
#include "Tunnel.h"

#include "utils.h"

#include <loguru.hpp>

Rat::Rat(const sf::Vector2f& Pos, const Tunnel* T) : Position(Pos), CurrTunnel(T)
{
  Size = {0.08f, 0.065f * 0.8f};
  DxDy = {0, 0};
  CurrState = State::Idle;

  RunAnim = std::make_unique<Animation>(std::vector<std::string> {
      "../../game/resources/Rats/RatRun1.png",
      "../../game/resources/Rats/RatRun2.png",
  });
  RunAnim->setLoop(true);
  RunAnim->setFrameTime(0.1);
  RunAnim->start();
}

void Rat::update(float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects,
                 const Player* Pl)
{
  if (!Visible)
    return;

  RunAnim->update();

  float FrameDrawingTimeS = FrameDrawingTimeMs / 1000;
  const auto& PlPos = Pl->getPosition();

  constexpr float Dist2ToSpotDimLight = 0.25f;
  constexpr float Dist2ToSpotBrightLight = 1.0f;
  constexpr float Dist2ToLoseDimLight = 1.5 * 1.5;
  constexpr float Dist2ToLoseBrightLight = 3 * 3;
  constexpr float Dist2ToGetScared = 0.25 * 0.25;
  constexpr float Dist2ToStopBeingScared = 1.3 * 1.3;

  float Dist2ToPlayer = (PlPos.x - Position.x) * (PlPos.x - Position.x);
  bool BrightLight = Pl->getLightSource()->getBaseIntensity() > 0.45f;

  switch (CurrState)
  {
  case State::Idle:
    if (TurnedLeft == PlPos.x < Position.x)
    {
      if ((Dist2ToPlayer <= Dist2ToSpotBrightLight) && BrightLight
          ||
          (Dist2ToPlayer <= Dist2ToSpotDimLight))
      {
        CurrState = State::PlayerSpotted;
        break;
      }
    }

    LOG_F(INFO, "State::Idle");
    LOG_F(INFO, "IterationsToRunInOneDirection: %d", IterationsToRunInOneDirection);

    if (IterationsToRunInOneDirection < 1)
    {
      TurnedLeft = !TurnedLeft;
      IterationsToRunInOneDirection = randIntInRange(100, 500);
    }
    else
    {
      IterationsToRunInOneDirection--;
      DxDy.x = TurnedLeft ? -0.1f : 0.1f;
    }
    break;

  case State::PlayerSpotted:
    if (BrightLight)
    {
      if (Dist2ToPlayer > Dist2ToLoseBrightLight)
      {
        CurrState = State::Idle;
        break;
      }
      if (Dist2ToPlayer < Dist2ToGetScared)
      {
        CurrState = State::ScaredByLight;
        break;
      }
    }
    else
    {
      if (Dist2ToPlayer > Dist2ToLoseDimLight)
        CurrState = State::Idle;
    }

    if (!PlayerWasDamagedRecently)
    {
       TurnedLeft = PlPos.x < Position.x;
       DxDy.x = TurnedLeft ? -0.2f : 0.2f;
    }
    else
    {
      TurnedLeft = PlPos.x > Position.x;
      DxDy.x = TurnedLeft ? -0.2f : 0.2f;
      IterationsToRunInOneDirection--;

      if (IterationsToRunInOneDirection < 1)
        PlayerWasDamagedRecently = false;
    }
    break;

  case State::ScaredByLight:
    LOG_F(INFO, "Dist2ToPlayer: %f", Dist2ToPlayer);
    LOG_F(INFO, "Dist2ToStopBeingScared: %f", Dist2ToStopBeingScared);

    if (Dist2ToPlayer > Dist2ToStopBeingScared)
    {
      CurrState = State::Idle;
      break;
    }

    TurnedLeft = PlPos.x > Position.x;
    DxDy.x = TurnedLeft ? -0.2f : 0.2f;
    break;
  }

  DxDy.y -= 6 * FrameDrawingTimeS;
  OnGround = false;

  move(Objects, FrameDrawingTimeS);
}

void Rat::move(const std::vector<SolidObject*>& Objects, float FrameDrawingTimeS)
{
  auto OldPos = Position;
  setPosition(Position + sf::Vector2f(DxDy.x * FrameDrawingTimeS, 0));

  auto CollObjects = getAllObjectsWithCollision(Objects);
  for (const auto& CollObj : CollObjects)
  {
    if (dynamic_cast<Tunnel*>(CollObj))
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
    else if (auto* Pl = dynamic_cast<Player*>(CollObj))
    {
      LOG_F(INFO, "Collision with Player!");
      setPosition(OldPos);

      Pl->damageByRat();
      PlayerWasDamagedRecently = true;
      IterationsToRunInOneDirection = randIntInRange(30, 50);

      break;
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
  if (!Visible)
    return;

  sf::Vector2f WorldTopLeft = Position + sf::Vector2f(-Size.x / 2, Size.y / 2);
  sf::Vector2f WorldBottomRight = Position + sf::Vector2f(Size.x / 2, -Size.y / 2);

  sf::Vector2f ScreenTopLeft = worldCoordsToScreen(WorldTopLeft, WorldWindowObj);
  sf::Vector2f ScreenBottomRight = worldCoordsToScreen(WorldBottomRight, WorldWindowObj);

  sf::Vector2f CenterScreenCoords = worldCoordsToScreen(Position, WorldWindowObj);

  sf::Vector2f ScreenBBSize = ScreenBottomRight - ScreenTopLeft;

  constexpr float CharacterWidthToBoundingBox = 2.0;
  float SpriteScale = ScreenBBSize.x / CharacterWidthPx * CharacterWidthToBoundingBox;
  Sprite.setTexture(*RunAnim->getFrames()[RunAnim->getCurrFrameIndex()]);
  Sprite.setScale(SpriteScale, SpriteScale);
  Sprite.setPosition(CenterScreenCoords -
                     sf::Vector2f(CharacterCenterXPx * SpriteScale,
                                  CharacterCenterYPx * SpriteScale));

  if (TurnedLeft)
  {
    Sprite.scale(-1, 1);
    Sprite.move(CharacterCenterXPx * 2 * SpriteScale, 0);
  }
  else
  {
    Sprite.scale(1, 1);
  }

  sf::RectangleShape Rect(ScreenBottomRight - ScreenTopLeft);
  Rect.setPosition(ScreenTopLeft);
  Rect.setOutlineThickness(5);
  Rect.setOutlineColor(sf::Color::Cyan);

  Win.draw(Rect); // Draw bounding box
  Win.draw(Sprite);
}

bool Rat::isCollision(const SolidObject* Other) const
{
  if (!Visible)
    return false;

  if (dynamic_cast<const Rat*>(Other))
    return false;
  if (const Player* P = dynamic_cast<const Player*>(Other))
    return P->isCollisionWithRat(this);
  if (const Tunnel* T = dynamic_cast<const Tunnel*>(Other))
    return T->isCollisionWithBoundingBox(Position, Size);

  return false;
}
