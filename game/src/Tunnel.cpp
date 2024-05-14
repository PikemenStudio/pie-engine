//
// Created by anton on 5/13/24.
//

#include "Tunnel.h"
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
