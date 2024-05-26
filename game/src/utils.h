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

sf::Vector2f worldCoordsToScreen(const sf::Vector2f& WorldCoords, const WorldWindow& WorldWindowObj);
//sf::Vector2f screenCoordsToWorld(const sf::Vector2f& ScreenCoords);

sf::Vector2f worldDeltaToScreen(const sf::Vector2f& WorldDelta, const WorldWindow& WorldWindowObj);

// A random number from 0 to 1
float rand01();

float randInRange(float From, float To);

// Including From and To
int randIntInRange(int From, int To);

inline sf::Vector2f operator*(sf::Vector2f A, float B)
{
  return sf::Vector2f(A.x * B, A.y * B);
}

inline sf::Vector2f operator*(float B, sf::Vector2f A)
{
  return A * B;
}

#endif // ENGINE_UTILS_H
