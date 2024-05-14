//
// Created by anton on 5/13/24.
//

#include "utils.h"

#include <sstream>

sf::Vector2f worldCoordsToScreen(const sf::Vector2f& WorldCoords, sf::FloatRect WorldWindow)
{
  sf::Vector2f WindowCenter = sf::Vector2f(WorldWindow.left + WorldWindow.width/2,
                                           WorldWindow.top - WorldWindow.height/2);
  sf::Vector2f WindowCoords = WorldCoords - WindowCenter;
  return sf::Vector2f((WindowCoords.x + 1) * ScreenWidth / 2, (1 - WindowCoords.y) * ScreenHeight / 2);
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