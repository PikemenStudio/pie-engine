//
// Created by anton on 5/13/24.
//

#include "utils.h"

#include <sstream>

sf::Vector2f worldCoordsToScreen(const sf::Vector2f& WorldCoords)
{
  return sf::Vector2f((WorldCoords.x + 1) * 800 / 2, (1 - WorldCoords.y) * 600 / 2);
}

sf::Vector2f screenCoordsToWorld(const sf::Vector2f& ScreenCoords)
{
  return sf::Vector2f((ScreenCoords.x - 400) / 400, -(ScreenCoords.y - 300) / 300);
}

std::string loadTextFile(const std::string& Name)
{
  std::ifstream Stream(Name);
  std::stringstream Buffer;
  Buffer << Stream.rdbuf();

  return Buffer.str();
}