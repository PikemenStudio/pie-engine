//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_FLOOR_H
#define ENGINE_FLOOR_H

#include <vector>

#include <SFML/Graphics.hpp>

class Floor
{
public:
  Floor(float StartX, float StepX, int PointsCount);
  Floor(float StartX, float EndX, float StepX);

  void draw(sf::RenderTarget& Win, sf::FloatRect WorldWindow) const;

  float getStartX() const { return StartX; }
  float getStepX() const { return StepX; }
  const std::vector<float>& getCoords() const { return WorldCoordsY; }

private:
  float StartX = 0.0;
  float StepX = 0.1;
  std::vector<float> WorldCoordsY;
};

#endif // ENGINE_FLOOR_H
