//
// Created by FullHat on 14/05/2024.
//
#include "Triangle.hpp"

Triangle::Triangle(std::array<glm::vec3, 3> &&Vertexes) {
  this->Vertexes = std::move(Vertexes);
}
std::vector<float> Triangle::getVertices() {
  std::vector<float> VertexesVector;

  for (auto &Vertex : this->Vertexes) {
    VertexesVector.push_back(Vertex.x);
    VertexesVector.push_back(Vertex.y);
    VertexesVector.push_back(Vertex.z);
  }

  return VertexesVector;
}

std::string Triangle::getType() {
  return "Triangle";
}

glm::mat4 Triangle::calculateTransformation() {
  glm::mat4 Result = glm::mat4(1.0f);
  Result = glm::translate(Result, this->Position);
  Result = Result * Rotation;
  Result = Result * glm::scale(Result, this->Scale);

  return Result;
}
