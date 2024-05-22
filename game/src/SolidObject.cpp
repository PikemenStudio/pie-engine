//
// Created by anton on 5/15/24.
//

#include "SolidObject.h"

SolidObject* SolidObject::checkCollisionWithObjects(const std::vector<SolidObject*>& Objects)
{
  for (auto *Obj : Objects)
  {
    if (isCollision(Obj))
      return Obj;
  }

  return nullptr;
}

std::set<SolidObject*> SolidObject::getAllObjectsWithCollision(
    const std::vector<SolidObject*>& Objects)
{
  std::set<SolidObject*> Ret;

  for (auto *Obj : Objects)
  {
    if (isCollision(Obj))
      Ret.insert(Obj);
  }

  return Ret;
}