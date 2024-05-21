//
// Created by FullHat on 14/05/2024.
//
#include "Triangle.hpp"

Triangle::Triangle(std::array<glm::vec3, 3> &&Vertexes) {
  this->Vertexes = std::move(Vertexes);
}
std::vector<uint8_t> Triangle::getVertices() {
  std::vector<uint8_t> VertexesVector;

  for (auto &Vertex : this->Vertexes) {
    VertexesVector.push_back(Vertex.x);
    VertexesVector.push_back(Vertex.y);
    VertexesVector.push_back(Vertex.z);
  }

  return VertexesVector;
}

BaseObject::ObjectTypes Triangle::getType() {
  return BaseObject::ObjectTypes::TRIANGLE;
}

glm::mat4 Triangle::calculateTransformation() {
  glm::mat4 Result = glm::mat4(1.0f);
  Result = glm::translate(Result, this->Position);

  Result *= Rotation;

  Result = glm::scale(Result, this->Scale);

  return Result;
}
