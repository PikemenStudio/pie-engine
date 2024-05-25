//
// Created by FullHat on 17/05/2024.
//

#ifndef ENGINE_SRC_MODULES_OBJECTS_SQUARE_HPP
#define ENGINE_SRC_MODULES_OBJECTS_SQUARE_HPP

#include "BaseObject.hpp"
#include "glm/glm.hpp"

class Square : public BaseObject {
public:
  Square(std::array<glm::vec3, 4> &&Vertexes);
  ~Square() = default;

  std::vector<float> getVertices() override;
  ObjectTypes getType() override;

  glm::mat4 calculateTransformation() override;

protected:
  std::array<glm::vec3, 4> Vertexes;
};

#endif // ENGINE_SRC_MODULES_OBJECTS_SQUARE_HPP
