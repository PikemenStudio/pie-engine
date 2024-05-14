//
// Created by FullHat on 14/05/2024.
//

#ifndef ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_BASEOBJECT_HPP
#define ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_BASEOBJECT_HPP

#include "glm/glm.hpp"
#include <vector>

class BaseObject {
public:
  BaseObject() = default;
  virtual ~BaseObject() = default;

  virtual std::vector<uint8_t> getVertices() = 0;
  virtual glm::vec3 getPosition() = 0;

  enum class ObjectTypes {
    TRIANGLE,
  };

  virtual ObjectTypes getType() = 0;

  virtual void moveBy(glm::vec3 Vector) = 0;
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_BASEOBJECT_HPP
