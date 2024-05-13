//
// Created by anton on 5/13/24.
//

#include "Floor.h"
#include "utils.h"

#include <noise/noise.h>

Floor::Floor(float StartX, float StepX, int PointsCount)
{
  noise::module::Perlin NoiseGen;
  NoiseGen.SetSeed(time(nullptr));

  this->StartX = StartX;
  this->StepX = StepX;
  float CurrX = StartX;

  while (PointsCount--)
  {
    WorldCoordsY.push_back((NoiseGen.GetValue(CurrX, 0, 0) - 1) / 2);
    CurrX += StepX;
  }
}

Floor::Floor(float StartX, float EndX, float StepX)
    : Floor(StartX, StepX, static_cast<int>((EndX - StartX) / StepX) + 1)
{
}

void Floor::draw(sf::RenderTarget &Win, sf::FloatRect WorldWindow) const
{
  for (int I = 0; I < WorldCoordsY.size() - 1; I++)
  {
    float WorldX1 = StartX + I * StepX;
    float WorldY1 = WorldCoordsY[I];
    float WorldX2 = StartX + (I + 1) * StepX;
    float WorldY2 = WorldCoordsY[I + 1];

    sf::Vector2f ScreenXY1 = worldCoordsToScreen(sf::Vector2f(WorldX1, WorldY1), WorldWindow);
    sf::Vector2f ScreenXY2 = worldCoordsToScreen(sf::Vector2f(WorldX2, WorldY2), WorldWindow);

    sf::ConvexShape FloorPart;
    FloorPart.setPointCount(4);

    FloorPart.setPoint(0, ScreenXY1);
    FloorPart.setPoint(1, ScreenXY2);
    FloorPart.setPoint(2, sf::Vector2f(ScreenXY2.x, ScreenHeight));
    FloorPart.setPoint(3, sf::Vector2f(ScreenXY1.x, ScreenHeight));

    FloorPart.setFillColor(sf::Color(50, 50, 50));

    Win.draw(FloorPart);
  }
}
