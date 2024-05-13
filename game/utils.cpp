//
// Created by anton on 5/13/24.
//

#include "utils.h"

sf::Vector2f worldCoordsToScreen(const sf::Vector2f& WorldCoords)
{
  return sf::Vector2f((WorldCoords.x + 1) * 800 / 2, (1 - WorldCoords.y) * 600 / 2);
}