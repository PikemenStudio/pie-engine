//
// Created by anton on 5/14/24.
//

#ifndef ENGINE_SOLIDOBJECT_H
#define ENGINE_SOLIDOBJECT_H

class SolidObject
{
public:
  virtual bool isCollision(const SolidObject* Other) const = 0;
};

#endif // ENGINE_SOLIDOBJECT_H
