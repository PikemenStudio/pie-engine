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

glm::vec3 Triangle::getPosition() { return this->Position; }

BaseObject::ObjectTypes Triangle::getType() {
  return BaseObject::ObjectTypes::TRIANGLE;
}

std::string Triangle::getDumpName() { return "TS"; }

void Triangle::moveBy(glm::vec3 Vector) { this->Position += Vector; }

glm::mat4 Triangle::calculateTransformation() {
  glm::mat4 Result = glm::mat4(1.0f);
  return Result;
}
