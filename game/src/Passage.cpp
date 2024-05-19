//
// Created by anton on 5/19/24.
//

#include "Passage.h"
#include "Tunnel.h"

#include "utils.h"

Passage::Passage(const Tunnel* T1, const Tunnel* T2, float X)
{
  Tex.loadFromFile("../../game/resources/cave_entrance_2.png");
//  Tex.loadFromFile("../../game/resources/entrance.png");

  Tunnel1 = T1;
  Tunnel2 = T2;
  XCoord = X;
}

void Passage::draw(sf::RenderTarget &Win, const WorldWindow &WorldWindowObj)
{
  auto CenterScreenCoords = worldCoordsToScreen({XCoord, -0.5}, WorldWindowObj);

  Sprite.setTexture(Tex);
  float SpriteScale = 0.5f;
  Sprite.setScale(SpriteScale, SpriteScale);
  Sprite.setPosition(CenterScreenCoords.x - Tex.getSize().x / 2 * SpriteScale,
                     CenterScreenCoords.y - Tex.getSize().y / 2 * SpriteScale);
  Win.draw(Sprite);
//  auto ScreenPt0 = worldCoordsToScreen({XCoord + 0.2f, -1}, WorldWindowObj);
//  auto ScreenPt1 = worldCoordsToScreen({XCoord - 0.2f, -1}, WorldWindowObj);
//  auto ScreenPt2 = worldCoordsToScreen({XCoord - 0.13f, 1}, WorldWindowObj);
//  auto ScreenPt3 = worldCoordsToScreen({XCoord + 0.13f, 1}, WorldWindowObj);

//  sf::ConvexShape Shape;
//  Shape.setPointCount(4);
//  Shape.setPoint(0, ScreenPt0);
//  Shape.setPoint(1, ScreenPt1);
//  Shape.setPoint(2, ScreenPt2);
//  Shape.setPoint(3, ScreenPt3);
//
//  Shape.setFillColor(sf::Color::Black);
//  Shape.setPosition(CenterScreenCoords);

//  sf::RectangleShape Shape;
//  Shape.setFillColor(sf::Color::Black);
//  Shape.setSize(worldDeltaToScreen({0.4f, 2.0f}, WorldWindowObj));
//  Shape.setPosition(CenterScreenCoords.x - Shape.getSize().x / 2,
//                    CenterScreenCoords.y - Shape.getSize().y / 2);
//

//  Win.draw(Shape);
}
