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
  Center.y = 0.0f;

  if (Center.x - Size.x / 2 < SceneBorderLeft)
    Center.x = SceneBorderLeft + Size.x / 2;
  else if (Center.x + Size.x / 2 > SceneBorderRight)
    Center.x = SceneBorderRight - Size.x / 2;
//  WorldWindow.left = (Lantern->getPosition().x - WorldWindow.width / 2);
//
//  if (WorldWindow.left < TunnelObj->getStartX())
//    WorldWindow.left = TunnelObj->getStartX();
//  else if (WorldWindow.left + WorldWindow.width > TunnelObj->getEndX())
//    WorldWindow.left = TunnelObj->getEndX() - WorldWindow.width;
//
//  WorldWindow.top = WorldWindow.height / 2;
}
