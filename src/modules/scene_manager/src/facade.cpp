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
  static_cast<SceneManager *>(Data)->addObject(Object);
}

template <SceneManagerDependenciesConcept Dep>
void scene_manager_facades::SceneManagerBaseImpl<Dep>::removeObject(
    const std::string &ObjectName) {
  static_cast<SceneManager *>(Data)->removeObject(ObjectName);
}

template <SceneManagerDependenciesConcept Dep>
std::unique_ptr<BaseIterator>
scene_manager_facades::SceneManagerBaseImpl<Dep>::begin() {
  return static_cast<SceneManager *>(Data)->begin();
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
void scene_manager_facades::SceneManagerBaseImpl<Dep>::setCamera(glm::vec3 From,
                                                                 glm::vec3 To) {
  static_cast<SceneManager *>(Data)->setCamera(From, To);
}

template <SceneManagerDependenciesConcept Dep>
std::vector<glm::mat4>
scene_manager_facades::SceneManagerBaseImpl<Dep>::getTransformations() {
  return static_cast<SceneManager *>(Data)->getTransformations();
}

template <SceneManagerDependenciesConcept Dep>
std::shared_ptr<BaseObject>
scene_manager_facades::SceneManagerBaseImpl<Dep>::getObjectByName(
    const std::string &Name) {
  return static_cast<SceneManager *>(Data)->getObjectByName(Name);
}

template <SceneManagerDependenciesConcept Dep>
void scene_manager_facades::SceneManagerBaseImpl<Dep>::addPointLight(
    std::shared_ptr<PointLight> Light) {
  static_cast<SceneManager *>(Data)->addPointLight(Light);
}

template <SceneManagerDependenciesConcept Dep>
std::shared_ptr<PointLight>
scene_manager_facades::SceneManagerBaseImpl<Dep>::getPointLight(
    std::string Name) {
  return static_cast<SceneManager *>(Data)->getPointLight(Name);
}

template <SceneManagerDependenciesConcept Dep>
std::vector<std::shared_ptr<PointLight>>
scene_manager_facades::SceneManagerBaseImpl<Dep>::getPointLights() {
  return static_cast<SceneManager *>(Data)->getPointLights();
}

template class scene_manager_facades::SceneManagerBaseImpl<
    scene_manager_facades::SceneManagerDependencies>;
