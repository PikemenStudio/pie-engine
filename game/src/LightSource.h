//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_LIGHTSOURCE_H
#define ENGINE_LIGHTSOURCE_H

#include <SFML/Graphics.hpp>

#include <chrono>

class Background;
class WorldWindow;

class LightSource
{
public:
  LightSource(float X, float Y, float Intens);

  void update();

  sf::Vector2f getPosition() const { return WorldCoords; }
  void setPosition(sf::Vector2f Pos) { WorldCoords = Pos; }

  void setIntensity(float I) { Intensity = I; }
  float getIntensity() const { return Intensity; }

  void setBaseIntensity(float I) { BaseIntensity = I; }
  float getBaseIntensity() const { return BaseIntensity; }
//  void draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj);

private:
  sf::Vector2f WorldCoords;
  float BaseIntensity;
  float Intensity;
  float IntensityIntervalMs;
  std::chrono::time_point<std::chrono::high_resolution_clock> IntervalStart;
};

#endif // ENGINE_LIGHTSOURCE_H
