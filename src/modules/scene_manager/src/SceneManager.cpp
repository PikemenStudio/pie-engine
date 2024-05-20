//
// Created by FullHat on 14/05/2024.
//

#include "SceneManager.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

SceneManager::SceneManager(SceneManagerProps Props) {}

void SceneManager::addObject(std::shared_ptr<BaseObject> Object) {
  Objects[Object->getDumpName()][Object->getType()].push_back(
      std::move(Object));
}

bool SceneManager::goToNextDump() {
  if (Objects.empty() || ObjectGetterIt == Objects.end()) {
    return false;
  }

  ObjectGetterIt++;
  TypeIt = ObjectGetterIt->second.begin();
  if (ObjectGetterIt == Objects.end()) {
    return false;
  }
  return true;
}

SceneManager::OneTypeObjects SceneManager::getNextObjects() {
  if (TypeIt == ObjectGetterIt->second.end()) {
    return {};
  }

  TypeIt++;
  if (TypeIt == ObjectGetterIt->second.end()) {
    return {};
  }
  OneTypeObjects OneTypeObjectsInstance = TypeIt->second;
  return OneTypeObjectsInstance;
}

SceneManager::OneTypeObjects SceneManager::getCurrentObjects() const {
  if (TypeIt == ObjectGetterIt->second.end()) {
    return {};
  }

  return TypeIt->second;
}

void SceneManager::resetObjectGetter() {
  ObjectGetterIt = Objects.begin();
  TypeIt = Objects.begin()->second.begin();
}

SceneManager::CameraData SceneManager::getCamera(glm::vec2 WindowSize) {
  glm::vec3 Eye(0.0f, 0.0f, 1.0f);
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
