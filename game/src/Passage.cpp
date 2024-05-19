//
// Created by anton on 5/19/24.
//

#include "Passage.h"
#include "Tunnel.h"

Passage::Passage(const Tunnel* T1, const Tunnel* T2, float X)
{
  Tunnel1 = T1;
  Tunnel2 = T2;
  XCoord = X;
}