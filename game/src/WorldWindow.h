//
// Created by anton on 5/14/24.
//

#ifndef ENGINE_WORLDWINDOW_H
#define ENGINE_WORLDWINDOW_H

#include <SFML/System/Vector2.hpp>

class WorldWindow
{
public:
  WorldWindow(sf::Vector2f Center, sf::Vector2f Size, float SceneBorderLeft, float SceneBorderRight);

  void updateByPlayerPos(sf::Vector2f Pos);

  sf::Vector2f getCenter() const { return Center; }
  sf::Vector2f getSize() const { return Size; }

private:
  sf::Vector2f Center, Size;
  float SceneBorderLeft, SceneBorderRight;
};

#endif // ENGINE_WORLDWINDOW_H
