//
// Created by anton on 5/13/24.
//

#include "Tunnel.h"
#include "Player.h"
#include "WorldWindow.h"
#include "utils.h"

#include <noise/noise.h>

#include <bits/stdc++.h>
#include <loguru.hpp>

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
  float T2 = (A.y * (B.x - A.x) + (B.y - A.y) * C.x - (B.y - A.y) * A.x - C.y * (B.x - A.x)) /
             ((D.y - C.y) * (B.x - A.x) - (B.y - A.y) * (D.x - C.x));
  if (std::isnan(T2) || T2 < 0 || T2 > 1)
    return false;

  float T1;
  if (B.x - A.x != 0.0f)
    T1 = (C.x + (D.x - C.x) * T2 - A.x) / (B.x - A.x);
  else if (B.y - A.y)
    T1 = (C.y + (D.y - C.y) * T2 - A.y) / (B.y - A.y);
  else
    return false;

  if (T1 < 0 || T1 > 1)
    return false;

  return true;
}

static void testLineSegmentsIntersection()
{
  sf::Vector2f Pt1(-0.5, 0);
  sf::Vector2f Pt2(0.5, 0);
  sf::Vector2f Pt3(0, 0.5);
  sf::Vector2f Pt4(0, -0.5);
  assert(lineSegmentsIntersect(Pt3, Pt4, Pt1, Pt2));
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
//  testLineSegmentsIntersection();
  const auto& PlPos = Pl->getPosition();
  const auto& PlSize = Pl->getSize();

  // player
  auto Pt1 = PlPos + sf::Vector2f(-PlSize.x / 2,  PlSize.y / 2);
  auto Pt2 = PlPos + sf::Vector2f( PlSize.x / 2,  PlSize.y / 2);
  auto Pt3 = PlPos + sf::Vector2f( PlSize.x / 2, -PlSize.y / 2);
  auto Pt4 = PlPos + sf::Vector2f(-PlSize.x / 2, -PlSize.y / 2);
  std::vector<sf::Vector2f> PlPoly = {Pt1, Pt2, Pt3, Pt4};

  // ceiling
  int LeftIndex = std::clamp(static_cast<int>((Pt1.x - StartX) / StepX), 0,
                             static_cast<int>(WorldCoordsYCeiling.size()-1));
  int RightIndex = std::clamp(static_cast<int>((Pt2.x - StartX) / StepX) + 1, 0,
                              static_cast<int>(WorldCoordsYCeiling.size()-1));
  std::vector<sf::Vector2f> CeilPoly;
  for (int I = LeftIndex; I <= RightIndex; I++)
  {
    float CurrYCeiling = WorldCoordsYCeiling[I];
    if (CurrYCeiling < PlPos.y)
      return true;
    CeilPoly.emplace_back(StartX + I * StepX, CurrYCeiling);
  }

  if (isPolylinesIntersection(PlPoly, CeilPoly))
    return true;

  // floor
  std::vector<sf::Vector2f> FloorPoly;
  for (int I = LeftIndex; I <= RightIndex; I++)
  {
    float CurrYFloor = WorldCoordsYFloor[I];
    if (CurrYFloor > PlPos.y)
      return true;
    FloorPoly.emplace_back(StartX + I * StepX, CurrYFloor);
  }

  return isPolylinesIntersection(PlPoly, FloorPoly);
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
