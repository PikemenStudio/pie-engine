//
// Created by FullHat on 25/05/2024.
//

#ifndef ENGINE_SRC_MODULES_OBJECTS_ACTOR_HPP
#define ENGINE_SRC_MODULES_OBJECTS_ACTOR_HPP

#include "BaseObject.hpp"
#include <array>

class Actor : public BaseObject {
public:
  Actor(const std::string &FileName);
  ~Actor() = default;

  std::vector<float> getVertices() override;
  std::vector<float> getColors() override;
  std::vector<float> getTextureCoords() override;
  std::vector<uint32_t> getIndexes();
  ObjectTypes getType() override { return ObjectTypes::ACTOR; }

  glm::mat4 calculateTransformation() override { return glm::mat4(1.0f); }

protected:
  std::vector<float> Vertexes, Normals, Colors;
  std::vector<float> TextureCoords;
  std::vector<uint32_t> Indexes, TextureIndexes;
};

#endif // ENGINE_SRC_MODULES_OBJECTS_ACTOR_HPP
