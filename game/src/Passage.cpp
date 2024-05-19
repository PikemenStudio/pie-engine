//
// Created by anton on 5/19/24.
//

#include "Passage.h"
#include "Tunnel.h"

#include "utils.h"

Passage::Passage(const Tunnel* T1, const Tunnel* T2, float X)
{
  Tunnel1 = T1;
  Tunnel2 = T2;
  XCoord = X;
}

void Passage::draw(sf::RenderTarget &Win, const WorldWindow &WorldWindowObj)
{
  sf::RectangleShape Rect;
  Rect.setSize({50, 50});

  auto ScreenCoords = worldCoordsToScreen({XCoord, 0}, WorldWindowObj);
  Rect.setFillColor(sf::Color::Black);
  Rect.setPosition(ScreenCoords - sf::Vector2f(Rect.getSize().x / 2, Rect.getSize().y / 2));

  Win.draw(Rect);
}
