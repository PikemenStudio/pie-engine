//
// Created by anton on 5/21/24.
//

#include "Rat.h"
#include "Player.h"
#include "Tunnel.h"

#include "utils.h"

Rat::Rat(const sf::Vector2f& Pos, const Tunnel* T) : Position(Pos), CurrTunnel(T)
{
  Size = {0.1f, 0.065f};
  DxDy = {0, 0};
//  CurrState = State::Idle;
  CurrState = State::Idle;
}

void Rat::update(float FrameDrawingTimeMs, const std::vector<SolidObject*>& Objects,
                 const Player* Pl)
{
  if (!Visible)
    return;

  float FrameDrawingTimeS = FrameDrawingTimeMs / 1000;
  const auto& PlPos = Pl->getPosition();

  constexpr float Dist2ToSpotDimLight = 0.25f;
  constexpr float Dist2ToSpotBrightLight = 1.0f;
  constexpr float Dist2ToLoseDimLight = 1.5 * 1.5;
  constexpr float Dist2ToLoseBrightLight = 3 * 3;

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
        CurrState = State::Idle;
    }
    else
    {
      if (Dist2ToPlayer > Dist2ToLoseDimLight)
        CurrState = State::Idle;
    }

    TurnedLeft = PlPos.x < Position.x;
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
  if (auto* CollObj = checkCollisionWithObjects(Objects))
  {
    auto PosBeforeSpaceSearch = Position;

    // try to go up and down a little
//    setPosition(Position + sf::Vector2f(0, 1.25 * DxDy.x * FrameDrawingTimeS));
    setPosition(Position + sf::Vector2f(0, 1.5 * DxDy.x * FrameDrawingTimeS));

    if (CollObj->isCollision(this))
    {
      setPosition(PosBeforeSpaceSearch);
//      setPosition(Position - sf::Vector2f(0, 1.25 * DxDy.x * FrameDrawingTimeS));
      setPosition(Position - sf::Vector2f(0, 1.5 * DxDy.x * FrameDrawingTimeS));

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
  if (!Visible)
    return;

  sf::RectangleShape Rect(worldDeltaToScreen(Size, WorldWindowObj));

  auto CenterOnScreen = worldCoordsToScreen(Position, WorldWindowObj);
  Rect.setPosition(CenterOnScreen - sf::Vector2f(Rect.getSize().x / 2, Rect.getSize().y / 2));
  Rect.setFillColor(sf::Color::Black);

  Win.draw(Rect);
}

bool Rat::isCollision(const SolidObject* Other) const
{
  if (!Visible)
    return false;

  if (dynamic_cast<const Rat*>(Other))
    return false;
  if (const Player* P = dynamic_cast<const Player*>(Other))
    return false;
//    return P->isCollisionWithRat(this);
  if (const Tunnel* T = dynamic_cast<const Tunnel*>(Other))
    return T->isCollisionWithBoundingBox(Position, Size);

  return false;
}
