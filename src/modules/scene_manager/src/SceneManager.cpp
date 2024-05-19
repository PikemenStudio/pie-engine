//
// Created by FullHat on 14/05/2024.
//

#include "SceneManager.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

SceneManager::SceneManager(SceneManagerProps Props) {}

std::string SceneManager::addObject(std::unique_ptr<BaseObject> Object) {
  std::hash<std::unique_ptr<BaseObject>> Hasher;
  auto Hash = std::to_string(Hasher(Object));

  Objects[Object->getDumpName()][Hash] = std::move(Object);

  return Hash;
}

void SceneManager::addObject(std::string Name,
                             std::unique_ptr<BaseObject> Object) {
  Objects[Object->getDumpName()][Name] = std::move(Object);
}

std::optional<SceneManager::ObjectData> SceneManager::getNextObject() {
  if (Objects.empty() || ObjectGetterIt == Objects[CurrentDumpName].end()) {
    return {};
  }

  ObjectData Data{
      .Vertexes = ObjectGetterIt->second->getVertices(),
      .Position = ObjectGetterIt->second->getPosition(),
      .DumpName = ObjectGetterIt->second->getDumpName(),
      .Name = ObjectGetterIt->first,
      .Type = ObjectGetterIt->second->getType(),
  };

  ObjectGetterIt++;
  return {Data};
}

void SceneManager::resetObjectGetter(const std::string &DumpName) {
  CurrentDumpName = DumpName;
  ObjectGetterIt = Objects[DumpName].begin();
}

SceneManager::CameraData SceneManager::getCamera(glm::vec2 WindowSize) {
  glm::vec3 Eye(0.0f, 0.0f, 10.0f);
  glm::vec3 Center(0.0f, 0.0f, 0.0f);
  glm::vec3 Up(0.0f, 1.0f, 0.0f);
  glm::mat4 View = glm::lookAt(Eye, Center, Up);

  glm::mat4 Projection =
      glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

  return CameraData{
      .View = View,
      .Projection = Projection,
      .ViewProjection = Projection * View,
  };
}
