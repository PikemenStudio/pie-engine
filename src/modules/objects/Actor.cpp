//
// Created by FullHat on 25/05/2024.
//

#include "Actor.hpp"
#include "tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

struct Vertex {
  glm::vec3 Position{};
  glm::vec3 Color{};
  glm::vec3 Normal{};
  glm::vec2 Uv{};

  bool operator==(const Vertex &Other) const {
    return Position == Other.Position && Color == Other.Color &&
           Normal == Other.Normal && Uv == Other.Uv;
  }
};

template <typename T, typename... Rest>
void hashCombine(std::size_t &Seed, const T &v, const Rest &...rest) {
  Seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
  (hashCombine(Seed, rest), ...);
};

namespace std {
template <> struct hash<Vertex> {
  size_t operator()(Vertex const &Vertex) const {
    size_t Seed = 0;
    hashCombine(Seed, Vertex.Position, Vertex.Color, Vertex.Normal, Vertex.Uv);
    return Seed;
  }
};
} // namespace std

Actor::Actor(const std::string &FileName) {
  tinyobj::attrib_t Attrib;
  std::vector<tinyobj::shape_t> Shapes;
  std::vector<tinyobj::material_t> Materials;
  std::string Warn, err;

  if (!tinyobj::LoadObj(&Attrib, &Shapes, &Materials, &Warn, &err,
                        FileName.c_str())) {
    throw std::runtime_error(Warn + err);
  }

  Vertexes.clear();
  Indexes.clear();

  std::unordered_map<Vertex, uint32_t> UniqueVertices{};
  for (const auto &shape : Shapes) {
    for (int I = shape.mesh.indices.size() - 1; I >= 0; --I) {
      const auto &Index = shape.mesh.indices[I];
      Vertex Vertex{};

      if (Index.vertex_index >= 0) {
        Vertex.Position = {
            Attrib.vertices[3 * Index.vertex_index + 0],
            Attrib.vertices[3 * Index.vertex_index + 1],
            Attrib.vertices[3 * Index.vertex_index + 2],
        };

        Vertex.Color = {
            Attrib.colors[3 * Index.vertex_index + 0],
            Attrib.colors[3 * Index.vertex_index + 1],
            Attrib.colors[3 * Index.vertex_index + 2],
        };
      }

      if (Index.normal_index >= 0) {
        Vertex.Normal = {
            Attrib.normals[3 * Index.normal_index + 0],
            Attrib.normals[3 * Index.normal_index + 1],
            Attrib.normals[3 * Index.normal_index + 2],
        };
      } else {
        throw "";
      }

      if (Index.texcoord_index >= 0) {
        Vertex.Uv = {
            Attrib.texcoords[2 * Index.texcoord_index + 0],
            Attrib.texcoords[2 * Index.texcoord_index + 1],
        };
      }

      if (UniqueVertices.count(Vertex) == 0) {
        UniqueVertices[Vertex] = static_cast<uint32_t>(Vertexes.size() / 3);
        Vertexes.push_back(Vertex.Position.x);
        Vertexes.push_back(Vertex.Position.y);
        Vertexes.push_back(Vertex.Position.z);

        Colors.push_back(Vertex.Color.x);
        Colors.push_back(Vertex.Color.y);
        Colors.push_back(Vertex.Color.z);

        TextureCoords.push_back(Vertex.Uv.x);
        TextureCoords.push_back(Vertex.Uv.y);

        Normals.push_back(Vertex.Normal.x);
        Normals.push_back(Vertex.Normal.y);
        Normals.push_back(Vertex.Normal.z);
      }
      Indexes.push_back(UniqueVertices[Vertex]);
    }
  }
}
std::vector<float> Actor::getVertices() { return Vertexes; }
std::vector<float> Actor::getColors() { return Colors; }
std::vector<float> Actor::getTextureCoords() { return TextureCoords; }
std::vector<uint32_t> Actor::getIndexes() { return Indexes; }
std::vector<float> Actor::getNormals() { return Normals; }
glm::mat4 Actor::calculateTransformation() {
  glm::mat4 Result = glm::mat4(1.0f);
  Result *= glm::rotate(Result, RotationAngles.x, glm::vec3(1.0f, 0.0f, 0.0f));
  Result *= glm::rotate(Result, RotationAngles.y, glm::vec3(0.0f, 1.0f, 0.0f));
  Result *= glm::rotate(Result, RotationAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));
  Result *= glm::translate(Result, this->Position);
  Result = Result * glm::scale(Result, this->Scale);
  return Result;
}

void Actor::update() {
  // Do nothing
}
