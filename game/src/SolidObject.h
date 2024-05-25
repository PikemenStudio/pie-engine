//
// Created by anton on 5/14/24.
//

#ifndef ENGINE_SOLIDOBJECT_H
#define ENGINE_SOLIDOBJECT_H

#include <set>
#include <vector>

// Abstract class of an object with collisions
class SolidObject
{
public:
  virtual bool isCollision(const SolidObject* Other) const = 0;

  // Check for collision with every object in vector
  // return nullptr if no collisions
  // return object pointer if has collision with object
  SolidObject* checkCollisionWithObjects(const std::vector<SolidObject*>& Objects);

  std::set<SolidObject*> getAllObjectsWithCollision(const std::vector<SolidObject*>& Objects);
};

#endif // ENGINE_SOLIDOBJECT_H
