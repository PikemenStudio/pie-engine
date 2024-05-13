//
// Created by anton on 5/13/24.
//

#include "Background.h"

Background::Background(float ScreenW, float ScreenH) : BackgrRect(sf::Vector2f(ScreenW, ScreenH))
{
  BackgrRect.setPosition(0, 0);
  BackgrRect.setFillColor(sf::Color::White);
}

void Background::draw(sf::RenderTarget &Win)
{
  Win.draw(BackgrRect);
}
