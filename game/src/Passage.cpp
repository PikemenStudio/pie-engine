//
// Created by anton on 5/19/24.
//

#include "Passage.h"
#include "DimmingTransition.h"
#include "Player.h"
#include "Tunnel.h"

#include "utils.h"

#include <iostream>

Passage::Passage(const Tunnel* T1, const Tunnel* T2, const Player* Pl, float X)
  : Tunnel1(T1), Tunnel2(T2), PlayerObj(Pl), XCoord(X)
{
  Tex.loadFromFile("../../game/resources/cave_entrance_2.png");

  Height = static_cast<float>(Tex.getSize().y) / Tex.getSize().x * PassageWidth;

  computeYCoord(T1);
  computeYCoord(T2);
}

bool Passage::isInInteractZone(const Player* Pl)
{
  Tunnel* PlTunnel = Pl->getCurrTunnel();

  if (PlTunnel != Tunnel1 && PlTunnel != Tunnel2)
    return false;

  const auto& PlPos = Pl->getPosition();
  return PlPos.x < XCoord + PassageWidth/2 && PlPos.x > XCoord - PassageWidth/2;
}

void Passage::runInteraction()
{
  if (Transition && !Transition->isPlaying())
    Transition->play();
}

void Passage::computeYCoord(const Tunnel* Tnl)
{
//  Tunnel* CurrTunnel = PlayerObj->getCurrTunnel();

  float LeftBottomYCoord = Tnl->getFloorYCoord(XCoord - PassageWidth / 2);
  float RightBottomYCoord = Tnl->getFloorYCoord(XCoord + PassageWidth / 2);
  YCoords[Tnl] = std::min(LeftBottomYCoord, RightBottomYCoord) + Height / 2;
}

void Passage::draw(sf::RenderTarget &Win, const WorldWindow &WorldWindowObj)
{
  const Tunnel* CurrT = PlayerObj->getCurrTunnel();

  if (CurrT != Tunnel1 && CurrT != Tunnel2)
    return;

  auto CenterScreenCoords = worldCoordsToScreen(
      {XCoord, YCoords[CurrT]},
      WorldWindowObj);
  auto SizePx = worldDeltaToScreen({PassageWidth, Height}, WorldWindowObj);

  float FitScale = SizePx.x / Tex.getSize().x;

  Sprite.setTexture(Tex);
  Sprite.setScale(FitScale, FitScale);
  Sprite.setPosition(CenterScreenCoords.x - SizePx.x / 2,
                     CenterScreenCoords.y - SizePx.y / 2);
  Win.draw(Sprite);
}
