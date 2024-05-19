//
// Created by anton on 5/19/24.
//

#ifndef ENGINE_INTERACTABLE_H
#define ENGINE_INTERACTABLE_H

class Player;

class Interactable
{
public:
  virtual bool isInInteractZone(const Player* Pl) = 0;
  virtual void runInteraction() = 0;
};

#endif // ENGINE_INTERACTABLE_H
