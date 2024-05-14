//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_LIGHTSOURCE_H
#define ENGINE_LIGHTSOURCE_H

#include <SFML/Graphics.hpp>

class Background;

class LightSource
{
public:
  LightSource(float X, float Y);

  sf::Vector2f getPosition() const { return WorldCoords; }
  void setPosition(sf::Vector2f Pos) { WorldCoords = Pos; }
  void draw(sf::RenderTarget& Win, sf::FloatRect WorldWindow);

private:
  sf::Vector2f WorldCoords;
};

#endif // ENGINE_LIGHTSOURCE_H
