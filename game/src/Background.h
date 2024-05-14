//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_BACKGROUND_H
#define ENGINE_BACKGROUND_H

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class WorldWindow;

class Background
{
public:
  Background(float ScreenW, float ScreenH);

  void draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj);

private:
  sf::RectangleShape BackgrRect;
};

#endif // ENGINE_BACKGROUND_H
