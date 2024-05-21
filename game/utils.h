//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_UTILS_H
#define ENGINE_UTILS_H

#include <cmath>
#include <fstream>
#include <string>

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

constexpr int ScreenWidth = 1920;
constexpr int ScreenHeight = 1080;
constexpr int HalfScreenWidth = ScreenWidth / 2;
constexpr int HalfScreenHeight = ScreenHeight / 2;

sf::Vector2f worldCoordsToScreen(const sf::Vector2f& WorldCoords, sf::FloatRect WorldWindow);
sf::Vector2f screenCoordsToWorld(const sf::Vector2f& ScreenCoords);

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