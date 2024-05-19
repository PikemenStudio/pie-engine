//
// Created by anton on 5/19/24.
//

#ifndef ENGINE_DRAWABLE_H
#define ENGINE_DRAWABLE_H

#include <SFML/Graphics.hpp>

class WorldWindow;

class Drawable
{
public:
  virtual void draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj) = 0;
};

#endif // ENGINE_DRAWABLE_H
