//
// Created by anton on 5/19/24.
//

#include "Passage.h"
#include "DimmingTransition.h"
#include "Player.h"
#include "Tunnel.h"

#include "utils.h"

#include <iostream>

Passage::Passage(const Tunnel* T1, const Tunnel* T2, const Tunnel* Current, float X)
  : Tunnel1(T1), Tunnel2(T1), CurrTunnel(Current), XCoord(X)
{
//  this->Transition = Transition;

  Tex.loadFromFile("../../game/resources/cave_entrance_2.png");

  Height = static_cast<float>(Tex.getSize().y) / Tex.getSize().x * PassageWidth;

  computeYCoord();
}

bool Passage::isInInteractZone(const Player* Pl)
{
  Tunnel* PlTunnel = Pl->getCurrTunnel();

  if (PlTunnel != CurrTunnel)
    return false;

  const auto& PlPos = Pl->getPosition();
  return PlPos.x < XCoord + PassageWidth/2 && PlPos.x > XCoord - PassageWidth/2;
}

void Passage::runInteraction()
{
  if (Transition && !Transition->isPlaying())
    Transition->play();
}

void Passage::computeYCoord()
{
  // PassageWidth
  float LeftBottomYCoord = CurrTunnel->getFloorYCoord(XCoord - PassageWidth / 2);
  float RightBottomYCoord = CurrTunnel->getFloorYCoord(XCoord + PassageWidth / 2);

  YCoord = std::min(LeftBottomYCoord, RightBottomYCoord) + Height / 2;
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
