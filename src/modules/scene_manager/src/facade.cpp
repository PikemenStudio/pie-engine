//
// Created by FullHat on 13/05/2024.
//

#include "../facades/facade.hpp"
#include "SceneManager.hpp"

template <SceneManagerDependenciesConcept Dep>
scene_manager_facades::SceneManagerBaseImpl<Dep>::SceneManagerBaseImpl(
    SceneManagerFacadeStructs::SceneManagerProps<Dep> &&Props) {
  Data = new SceneManager({});
}

template <SceneManagerDependenciesConcept Dep>
scene_manager_facades::SceneManagerBaseImpl<Dep>::~SceneManagerBaseImpl() {
  auto *NativeData = static_cast<SceneManager *>(Data);

  delete NativeData;
}

template <SceneManagerDependenciesConcept Dep>
std::string scene_manager_facades::SceneManagerBaseImpl<Dep>::addObject(
    std::unique_ptr<BaseObject> Object) {
  return static_cast<SceneManager *>(Data)->addObject(std::move(Object));
}

template <SceneManagerDependenciesConcept Dep>
void scene_manager_facades::SceneManagerBaseImpl<Dep>::addObject(
    std::string Name, std::unique_ptr<BaseObject> Object) {
  static_cast<SceneManager *>(Data)->addObject(Name, std::move(Object));
}

template <SceneManagerDependenciesConcept Dep>
std::optional<SceneManagerFacadeStructs::ObjectData>
scene_manager_facades::SceneManagerBaseImpl<Dep>::getNextObject() {
  auto LocalObjectData = static_cast<SceneManager *>(Data)->getNextObject();
  if (LocalObjectData.has_value()) {
    return {SceneManagerFacadeStructs::ObjectData{
        .Vertexes = LocalObjectData->Vertexes,
        .Position = LocalObjectData->Position,
    }};
  }
  return {};
}

template <SceneManagerDependenciesConcept Dep>
void scene_manager_facades::SceneManagerBaseImpl<Dep>::resetObjectGetter() {
  static_cast<SceneManager *>(Data)->resetObjectGetter();
}

template class scene_manager_facades::SceneManagerBaseImpl<
    scene_manager_facades::SceneManagerDependencies>;
