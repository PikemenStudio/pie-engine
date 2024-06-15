//
// Created by FullHat on 14/05/2024.
//

#include "SceneManager.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

SceneManager::SceneManager(SceneManagerProps Props) {}

void SceneManager::addObject(std::shared_ptr<BaseObject> Object) {
  MultiShaderSetObjects[Object->getShaderSetName()][Object->getDumpName()]
                       [Object->getType()][Object->getTextureName()]
                           .push_back(Object);
}

void SceneManager::removeObject(const std::string &ObjectName) {
  for (auto &[ShaderSetName, Objects] : MultiShaderSetObjects) {
    for (auto &[DumpName, MultiTypeObjects] : Objects) {
      for (auto &[Type, OneTypeObjects] : MultiTypeObjects) {
        for (auto &Object : OneTypeObjects) {
          const auto It =
              std::find_if(Object.second.begin(), Object.second.end(),
                           [&](auto &ObjectWithTexture) {
                             return ObjectWithTexture->getName() == ObjectName;
                           });
          if (It != Object.second.end()) {
            Object.second.erase(It);
            return;
          }
        }
      }
    }
  }
}

SceneManager::CameraData SceneManager::getCamera(glm::vec2 WindowSize) {
  glm::vec3 Eye(From);
  glm::vec3 Center(To);
  glm::vec3 Up(0.0f, 0.0f, 1.0f);
  glm::mat4 View = glm::lookAt(Eye, Center, Up);

  glm::mat4 Projection =
      glm::perspective(glm::radians(45.0f), 3456.0f / 2234.0f, 0.1f, 100.0f);
  Projection[1][1] *= -1;

  return CameraData{
      .View = View,
      .Projection = Projection,
      .ViewProjection = Projection * View,
  };
}
void SceneManager::setCamera(glm::vec3 From, glm::vec3 To) {
  this->From = From;
  this->To = To;
}
