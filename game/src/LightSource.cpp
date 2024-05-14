//
// Created by anton on 5/13/24.
//

#include "LightSource.h"
#include "Background.h"

#include "utils.h"

#include <chrono>

using namespace std::chrono;

LightSource::LightSource(float X, float Y, float Intens)
    : WorldCoords(X, Y), Intensity(Intens)
{
  IntensityIntervalMs = 300.0f;
  IntervalStart = high_resolution_clock::now();
}

void LightSource::update()
{
  auto MsFromIntStart = duration_cast<milliseconds>((high_resolution_clock::now() - IntervalStart)).count();

  if (MsFromIntStart > IntensityIntervalMs)
  {
    // generate new interval
    // change intensity
//    Intensity = 0.45 + static_cast<float>(rand() % 1000) / 1000 * 0.03;
    IntensityIntervalMs = 50 + static_cast<float>(rand() % 1000) / 1000 * 250;
    IntervalStart = high_resolution_clock::now();
  }
}

//void LightSource::draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj)
//{
//  sf::CircleShape DummySource(25);
//
//  auto ScreenCoords = worldCoordsToScreen(WorldCoords, WorldWindowObj);
//  DummySource.setPosition(ScreenCoords - sf::Vector2f(25, 25));
//  DummySource.setFillColor(sf::Color::White);
//  DummySource.setOutlineThickness(5);
//  DummySource.setOutlineColor(sf::Color::Black);
//
//  Win.draw(DummySource);
//}
