//
// Created by anton on 5/13/24.
//

#include "LightSource.h"
#include "Background.h"

#include "utils.h"

LightSource::LightSource(float X, float Y) : WorldCoords(X, Y)
{
}

void LightSource::draw(sf::RenderTarget& Win)
{
  sf::CircleShape DummySource(5);
  DummySource.setPosition(worldCoordsToScreen(WorldCoords));
  DummySource.setFillColor(sf::Color::Cyan);

  Win.draw(DummySource);
}
