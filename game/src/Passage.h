//
// Created by anton on 5/19/24.
//

#ifndef ENGINE_PASSAGE_H
#define ENGINE_PASSAGE_H

#include "Drawable.h"

class Tunnel;

class Passage : public Drawable
{
public:
  Passage(const Tunnel* T1, const Tunnel* T2, float X);

  void draw(sf::RenderTarget& Win, const WorldWindow& WorldWindowObj) override;

private:
  const Tunnel* Tunnel1, * Tunnel2;
  float XCoord;

  sf::Texture Tex;
  sf::Sprite Sprite;
};

#endif // ENGINE_PASSAGE_H
