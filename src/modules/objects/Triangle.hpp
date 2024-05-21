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

  std::vector<uint8_t> getVertices() override;
  glm::vec3 getPosition() override;
  ObjectTypes getType() override;
  std::string getDumpName() override;

  void moveBy(glm::vec3 Vector) override;
  void rotateBy(glm::vec3 Axis, float Angle) override;
  void scaleBy(glm::vec3 Vector) override;

  glm::mat4 calculateTransformation() override;

protected:
  std::array<glm::vec3, 3> Vertexes;
  glm::vec3 Position;
  glm::mat4 Rotation;
  glm::vec3 Scale = {1.0f, 1.0f, 1.0f};
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_TRIANGLE_HPP
