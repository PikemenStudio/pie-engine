//
// Created by anton on 5/13/24.
//

#include "utils.h"
#include "WorldWindow.h"

#include <sstream>

sf::Vector2f worldCoordsToScreen(const sf::Vector2f& WorldCoords, const WorldWindow& WorldWindowObj)
{
  sf::Vector2f WindowCenter = WorldWindowObj.getCenter();
  float WinW = WorldWindowObj.getSize().x;
  float WinH = WorldWindowObj.getSize().y;
  sf::Vector2f WindowCoords = WorldCoords - WindowCenter;

  return sf::Vector2f((WindowCoords.x + WinW/2) / WinW * ScreenWidth,
                      (WinH/2 - WindowCoords.y) / WinH * ScreenHeight);
}

sf::Vector2f worldDeltaToScreen(const sf::Vector2f& WorldDelta, const WorldWindow& WorldWindowObj)
{
  float PixelsInUnitX = ScreenWidth / WorldWindowObj.getSize().x;
  float PixelsInUnitY = ScreenHeight / WorldWindowObj.getSize().y;

  return {WorldDelta.x * PixelsInUnitX, WorldDelta.y * PixelsInUnitY};
}

//sf::Vector2f screenCoordsToWorld(const sf::Vector2f& ScreenCoords)
//{
//  return sf::Vector2f((ScreenCoords.x - HalfScreenWidth) / HalfScreenWidth,
//                      -(ScreenCoords.y - HalfScreenHeight) / HalfScreenHeight);
//}

std::string loadTextFile(const std::string& Name)
{
  std::ifstream Stream(Name);
  std::stringstream Buffer;
  Buffer << Stream.rdbuf();

  return Buffer.str();
}