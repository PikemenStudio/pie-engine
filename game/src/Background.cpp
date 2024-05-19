//
// Created by anton on 5/13/24.
//

#include "Background.h"
#include "WorldWindow.h"

Background::Background(float ScreenW, float ScreenH) : BackgrRect(sf::Vector2f(ScreenW, ScreenH))
{
  BackgrRect.setPosition(0, 0);
  BackgrRect.setFillColor(sf::Color::White);
//  BackgrRect.setFillColor(sf::Color(100, 100, 100));
}

void Background::draw(sf::RenderTarget &Win, const WorldWindow& WorldWindowObj)
{
  (void)WorldWindowObj;
  Win.draw(BackgrRect);
}
