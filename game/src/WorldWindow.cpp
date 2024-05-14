//
// Created by anton on 5/14/24.
//

#include "WorldWindow.h"

WorldWindow::WorldWindow(sf::Vector2f Center, sf::Vector2f Size,
                         float SceneBorderLeft, float SceneBorderRight)
{
  this->Center = Center;
  this->Size = Size;
  this->SceneBorderLeft = SceneBorderLeft;
  this->SceneBorderRight = SceneBorderRight;
}

void WorldWindow::updateByPlayerPos(sf::Vector2f Pos)
{
  Center.x = Pos.x;
  Center.y = Size.y / 2;

  if (Center.x - Size.x < SceneBorderLeft)
    Center.x = SceneBorderLeft + Size.x;
  else if (Center.x + Size.x > SceneBorderRight)
    Center.x = SceneBorderRight - Size.x;
//  WorldWindow.left = (Lantern->getPosition().x - WorldWindow.width / 2);
//
//  if (WorldWindow.left < FloorObj->getStartX())
//    WorldWindow.left = FloorObj->getStartX();
//  else if (WorldWindow.left + WorldWindow.width > FloorObj->getEndX())
//    WorldWindow.left = FloorObj->getEndX() - WorldWindow.width;
//
//  WorldWindow.top = WorldWindow.height / 2;
}
