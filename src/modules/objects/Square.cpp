//
// Created by FullHat on 17/05/2024.
//

#include "Square.hpp"

Square::Square(std::array<glm::vec3, 4> &&Vertexes) {
  this->Vertexes = std::move(Vertexes);
}

std::vector<uint8_t> Square::getVertices() {
  std::vector<uint8_t> VertexesVector;

  for (auto &Vertex : this->Vertexes) {
    VertexesVector.push_back(Vertex.x);
    VertexesVector.push_back(Vertex.y);
    VertexesVector.push_back(Vertex.z);
  }

  return VertexesVector;
}

BaseObject::ObjectTypes Square::getType() {
  return BaseObject::ObjectTypes::SQUARE;
}

glm::mat4 Square::calculateTransformation() {
  glm::mat4 Result = glm::mat4(1.0f);
  Result = glm::translate(Result, this->Position);

  Result *= this->Rotation;

  Result = glm::scale(Result, this->Scale);

  return Result;
}