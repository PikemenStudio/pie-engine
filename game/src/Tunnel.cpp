//
// Created by anton on 5/13/24.
//

#include "Tunnel.h"
#include "Player.h"
#include "WorldWindow.h"
#include "utils.h"

#include <noise/noise.h>

static float getNoiseValFrom0To1(const noise::module::Module& NoiseGen, float CurrX)
{
  return (std::clamp(NoiseGen.GetValue(CurrX, 0, 0), -1.0, 1.0) + 1) / 2;
}

Tunnel::Tunnel(float StartX, float StepX, int PointsCount)
{
  noise::module::Perlin NoiseGenFloor;
  NoiseGenFloor.SetSeed(time(nullptr));
  noise::module::Perlin NoiseGenCeiling;
  NoiseGenCeiling.SetSeed(time(nullptr) + 100);

  this->StartX = StartX;
  this->StepX = StepX;
  float CurrX = StartX;

  float FloorTop = -0.5, FloorBottom = -0.95;
  float CeilingTop = 0.95, CeilingBottom = -0.5;

  while (PointsCount--)
  {
    float NoiseVal = getNoiseValFrom0To1(NoiseGenFloor, CurrX);
    float FloorY = FloorBottom + NoiseVal * (FloorTop - FloorBottom);
    WorldCoordsYFloor.push_back(FloorY);

    NoiseVal = getNoiseValFrom0To1(NoiseGenCeiling, CurrX);
    float CeilY = CeilingBottom + NoiseVal * (CeilingTop - CeilingBottom);
    WorldCoordsYCeiling.push_back(CeilY);

    CurrX += StepX;
  }
}

Tunnel::Tunnel(float StartX, float EndX, float StepX)
    : Tunnel(StartX, StepX, static_cast<int>((EndX - StartX) / StepX) + 1)
{
}

bool Tunnel::isCollision(const SolidObject* Other) const
{
  if (dynamic_cast<const Tunnel*>(Other))
    return false;
  if (const Player* Pl = dynamic_cast<const Player*>(Other))
    return isCollisionWithPlayer(Pl);

  return false;
}

// line segments: [A, B] and [C, D]
static bool lineSegmentsIntersect(sf::Vector2f A, sf::Vector2f B, sf::Vector2f C, sf::Vector2f D)
{
  auto K = (B.y - A.y) / (B.x - A.x);

  auto DenomForT2 = (D.y - C.y - K*(D.x - C.x));
  if (DenomForT2 == 0)
    return false;

  auto T2 = (A.y + K*C.x - K*A.x - C.y) / DenomForT2;
  if (T2 < 0 || T2 > 1)
    return false;

  auto DenomForT1 = (B.x - A.x);
  if (DenomForT1 == 0)
    return false;

  auto T1 = (C.x + (D.x - C.x)*T2 - A.x) / DenomForT1;
  if (T1 < 0 || T1 > 1)
    return false;

  return true;
}

static bool isPolylinesIntersection(const std::vector<sf::Vector2f>& Poly1,
                                    const std::vector<sf::Vector2f>& Poly2)
{
  for (int I = 0; I < Poly1.size() - 1; I++)
  {
    for (int J = 0; J < Poly2.size() - 1; J++)
    {
      if (lineSegmentsIntersect(Poly1[I], Poly1[I+1], Poly2[J], Poly2[J+1]))
      {
        return true;
      }
    }
  }

  return false;
}

bool Tunnel::isCollisionWithPlayer(const Player* Pl) const
{
//  std::vector<sf::Vector2f> Poly1 = {
//      sf::Vector2f(0, 0),
//      sf::Vector2f(0.5, 1),
//      sf::Vector2f(1, 0),
//  };
//  std::vector<sf::Vector2f> Poly2 = {
//      sf::Vector2f(-1, 0),
//      sf::Vector2f(0, 1),
//      sf::Vector2f(0, 0.5),
//  };
  // ceiling
  auto Pt1 = Pl->getPosition() + sf::Vector2f(-Pl->getSize().x / 2, Pl->getSize().y / 2);
  auto Pt2 = Pl->getPosition() + sf::Vector2f( Pl->getSize().x / 2, Pl->getSize().y / 2);
  std::vector<sf::Vector2f> PlPoly = {Pt1, Pt2};

  int CeilLeftIndex = static_cast<int>((Pt1.x - StartX) / StepX);
  int CeilRightIndex = static_cast<int>((Pt2.x - StartX) / StepX) + 1;
  std::vector<sf::Vector2f> CeilPoly;
  for (int I = CeilLeftIndex; I <= CeilRightIndex && I < WorldCoordsYCeiling.size(); I++)
    CeilPoly.emplace_back(StartX + I * StepX, WorldCoordsYCeiling[I]);

  bool HasIntersection = isPolylinesIntersection(PlPoly, CeilPoly);

  return HasIntersection;
}

void Tunnel::draw(sf::RenderTarget &Win, const WorldWindow& WorldWindowObj) const
{
  drawTunnelPart(Win, WorldWindowObj, WorldCoordsYFloor, true);
  drawTunnelPart(Win, WorldWindowObj, WorldCoordsYCeiling, false);
}

void Tunnel::drawTunnelPart(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj,
                            const std::vector<float>& WorldCoordsY, bool IsFloor) const
{
  sf::ConvexShape Trapezium;
  Trapezium.setPointCount(4);
  Trapezium.setFillColor(sf::Color(50, 50, 50));

  float ScreenY = IsFloor ? ScreenHeight : 0;

  for (int I = 0; I < WorldCoordsY.size() - 1; I++)
  {
    sf::Vector2f WorldPt1(StartX + I * StepX, WorldCoordsY[I]);
    sf::Vector2f WorldPt2(StartX + (I + 1) * StepX, WorldCoordsY[I + 1]);

    sf::Vector2f ScreenXY1 = worldCoordsToScreen(WorldPt1, WorldWindowObj);
    sf::Vector2f ScreenXY2 = worldCoordsToScreen(WorldPt2, WorldWindowObj);

    Trapezium.setPoint(0, ScreenXY1);
    Trapezium.setPoint(1, ScreenXY2);
    Trapezium.setPoint(2, sf::Vector2f(ScreenXY2.x, ScreenY));
    Trapezium.setPoint(3, sf::Vector2f(ScreenXY1.x, ScreenY));

    Win.draw(Trapezium);
  }
}
