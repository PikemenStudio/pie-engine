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

  Tex.loadFromFile("../../game/resources/cave_entrance_2.png");
//  Tex.loadFromFile("../../game/resources/entrance.png");

  Height = static_cast<float>(Tex.getSize().y) / Tex.getSize().x * PassageWidth;
  YCoord = -0.5; // should be computed
}

void Passage::draw(sf::RenderTarget &Win, const WorldWindow &WorldWindowObj)
{
  auto CenterScreenCoords = worldCoordsToScreen(
      {XCoord, YCoord},
      WorldWindowObj);
  auto SizePx = worldDeltaToScreen({PassageWidth, Height}, WorldWindowObj);

  float FitScale = SizePx.x / Tex.getSize().x;

  Sprite.setTexture(Tex);
  Sprite.setScale(FitScale, FitScale);
  Sprite.setPosition(CenterScreenCoords.x - SizePx.x / 2,
                     CenterScreenCoords.y - SizePx.y / 2);
  Win.draw(Sprite);
}
