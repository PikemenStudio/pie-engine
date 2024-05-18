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

  std::vector<uint8_t> getVertices() override;
  glm::vec3 getPosition() override;
  ObjectTypes getType() override;
  std::string getDumpName() override;

  void moveBy(glm::vec3 Vector) override;

protected:
  std::array<glm::vec3, 4> Vertexes;
  glm::vec3 Position;
};

#endif // ENGINE_SRC_MODULES_OBJECTS_SQUARE_HPP
