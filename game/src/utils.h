//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_UTILS_H
#define ENGINE_UTILS_H

#include <cmath>
#include <fstream>
#include <string>

#include <SFML/System/Vector2.hpp>

class WorldWindow;

constexpr int ScreenWidth = 1366;
constexpr int ScreenHeight = 768;
constexpr int HalfScreenWidth = ScreenWidth / 2;
constexpr int HalfScreenHeight = ScreenHeight / 2;

sf::Vector2f worldCoordsToScreen(const sf::Vector2f& WorldCoords, const WorldWindow& WorldWindowObj);
//sf::Vector2f screenCoordsToWorld(const sf::Vector2f& ScreenCoords);

sf::Vector2f worldDeltaToScreen(const sf::Vector2f& WorldDelta, const WorldWindow& WorldWindowObj);

template <typename T>
T vectorLen(const sf::Vector2<T>& Vec)
{
  return std::sqrt(Vec.x * Vec.x + Vec.y * Vec.y);
}

template <typename T>
T vectorLenSquared(const sf::Vector2<T>& Vec)
{
  return Vec.x * Vec.x + Vec.y * Vec.y;
}

std::string loadTextFile(const std::string& Name);

#endif // ENGINE_UTILS_H
