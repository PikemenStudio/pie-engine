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

//  void draw();

private:
  const Tunnel* Tunnel1, * Tunnel2;
  float XCoord;
};

#endif // ENGINE_PASSAGE_H
