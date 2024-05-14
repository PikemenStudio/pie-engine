//
// Created by anton on 5/13/24.
//

#include "LightSource.h"
#include "Background.h"

#include "utils.h"

LightSource::LightSource(float X, float Y) : WorldCoords(X, Y)
{
}

//void LightSource::draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj)
//{
//  sf::CircleShape DummySource(25);
//
//  auto ScreenCoords = worldCoordsToScreen(WorldCoords, WorldWindowObj);
//  DummySource.setPosition(ScreenCoords - sf::Vector2f(25, 25));
//  DummySource.setFillColor(sf::Color::White);
//  DummySource.setOutlineThickness(5);
//  DummySource.setOutlineColor(sf::Color::Black);
//
//  Win.draw(DummySource);
//}
