//
// Created by FullHat on 14/05/2024.
//

#include "SceneManager.hpp"

SceneManager::SceneManager(SceneManagerProps Props) {
  ObjectGetterIt = Objects.begin();
}

std::string SceneManager::addObject(std::unique_ptr<BaseObject> Object) {
  std::hash<std::unique_ptr<BaseObject>> Hasher;
  auto Hash = std::to_string(Hasher(Object));

  Objects[Hash] = std::move(Object);

  return Hash;
}

void SceneManager::addObject(std::string Name,
                             std::unique_ptr<BaseObject> Object) {
  Objects[Name] = std::move(Object);
}

std::optional<SceneManager::ObjectData> SceneManager::getNextObject() {
  if (Objects.empty() || ObjectGetterIt == Objects.end()) {
    return {};
  }

  ObjectData Data{
      .Vertexes = ObjectGetterIt->second->getVertices(),
      .Position = ObjectGetterIt->second->getPosition(),
  };

  ObjectGetterIt++;
  return {Data};
}

void SceneManager::resetObjectGetter() { ObjectGetterIt = Objects.begin(); }
