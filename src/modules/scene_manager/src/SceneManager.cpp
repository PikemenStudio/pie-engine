//
// Created by FullHat on 14/05/2024.
//

#include "SceneManager.hpp"

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
