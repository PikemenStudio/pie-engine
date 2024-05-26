//
// Created by anton on 5/19/24.
//

#ifndef ENGINE_PASSAGE_H
#define ENGINE_PASSAGE_H

#include "Drawable.h"
#include "Interactable.h"

class Tunnel;
class Player;
class DimmingTransition;

class Passage : public Drawable, public Interactable
{
public:
  Passage(const Tunnel* T1, const Tunnel* T2, const Player* Pl, float X);

  void setTransition(DimmingTransition* T)
  {
    this->Transition = T;
  }

  bool isInInteractZone(const Player* Pl) override;
  void runInteraction() override;
  void draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj) override;

private:
  void computeYCoord(const Tunnel* Tnl);

  const Tunnel* Tunnel1, * Tunnel2;
  const Player* PlayerObj;
  DimmingTransition* Transition = nullptr;
  float XCoord;
  std::map<const Tunnel*, float> YCoords;
  float Height;

  static constexpr float PassageWidth = 0.55f;
//  static constexpr float ScaleFactor = 0.5f;

  sf::Texture Tex;
  sf::Sprite Sprite;
};

#endif // ENGINE_PASSAGE_H
