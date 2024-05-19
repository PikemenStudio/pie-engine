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
void scene_manager_facades::SceneManagerBaseImpl<Dep>::addObject(
    std::shared_ptr<BaseObject> Object) {
  static_cast<SceneManager *>(Data)->addObject(std::move(Object));
}

template <SceneManagerDependenciesConcept Dep>
bool scene_manager_facades::SceneManagerBaseImpl<Dep>::goToNextDump() {
  return static_cast<SceneManager *>(Data)->goToNextDump();
}

template <SceneManagerDependenciesConcept Dep>
SceneManagerFacadeStructs::OneTypeObjects
scene_manager_facades::SceneManagerBaseImpl<Dep>::getNextObjects() {
  auto OneTypeObjects = static_cast<SceneManager *>(Data)->getNextObjects();
  return OneTypeObjects;
}

template <SceneManagerDependenciesConcept Dep>
SceneManagerFacadeStructs::OneTypeObjects
scene_manager_facades::SceneManagerBaseImpl<Dep>::getCurrentObjects() const {
  auto OneTypeObjects = static_cast<SceneManager *>(Data)->getCurrentObjects();
  return OneTypeObjects;
}

template <SceneManagerDependenciesConcept Dep>
void scene_manager_facades::SceneManagerBaseImpl<Dep>::resetObjectGetter() {
  static_cast<SceneManager *>(Data)->resetObjectGetter();
}

template <SceneManagerDependenciesConcept Dep>
SceneManagerFacadeStructs::CameraData
scene_manager_facades::SceneManagerBaseImpl<Dep>::getCamera(
    glm::vec2 WindowSize) {
  auto LocalCameraData =
      static_cast<SceneManager *>(Data)->getCamera(WindowSize);
  return {SceneManagerFacadeStructs::CameraData{
      .View = LocalCameraData.View,
      .Projection = LocalCameraData.Projection,
      .ViewProjection = LocalCameraData.ViewProjection,
  }};
}

template <SceneManagerDependenciesConcept Dep>
std::vector<glm::mat4>
scene_manager_facades::SceneManagerBaseImpl<Dep>::getTransformations() {
  return static_cast<SceneManager *>(Data)->getTransformations();
}

template class scene_manager_facades::SceneManagerBaseImpl<
    scene_manager_facades::SceneManagerDependencies>;
