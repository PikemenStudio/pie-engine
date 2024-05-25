//
// Created by FullHat on 14/05/2024.
//

#ifndef ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_TRIANGLE_HPP
#define ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_TRIANGLE_HPP

#include "BaseObject.hpp"
#include <array>

class Triangle : public BaseObject {
public:
  Triangle(std::array<glm::vec3, 3> &&Vertexes);
  ~Triangle() = default;

  std::vector<float> getVertices() override;
  ObjectTypes getType() override;

  glm::mat4 calculateTransformation() override;

protected:
  std::array<glm::vec3, 3> Vertexes;
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_TRIANGLE_HPP
