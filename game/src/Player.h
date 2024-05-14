//
// Created by anton on 5/14/24.
//

#ifndef ENGINE_PLAYER_H
#define ENGINE_PLAYER_H

#include <SFML/Graphics.hpp>

#include "WorldWindow.h"
#include "utils.h"

class Player
{
public:
  Player(sf::Vector2f Center, sf::Vector2f Size)
  {
    this->Center = Center;
    this->Size = Size;
  }

  void draw(sf::RenderTarget &Win, const WorldWindow& WorldWindowObj)
  {

    sf::Vector2f WorldTopLeft = Center + sf::Vector2f(-Size.x / 2, Size.y / 2);
    sf::Vector2f WorldBottomRight = Center + sf::Vector2f(Size.x / 2, -Size.y / 2);

    sf::Vector2f ScreenTopLeft = worldCoordsToScreen(WorldTopLeft, WorldWindowObj);
    sf::Vector2f ScreenBottomRight = worldCoordsToScreen(WorldBottomRight, WorldWindowObj);

    sf::RectangleShape Rect(ScreenBottomRight - ScreenTopLeft);
    Rect.setPosition(ScreenTopLeft);
    Rect.setFillColor(sf::Color::Cyan);

    Win.draw(Rect);
  }

private:
  sf::Vector2f Center;
  sf::Vector2f Size;
};

#endif // ENGINE_PLAYER_H
