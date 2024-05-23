//
// Created by FullHat on 14/05/2024.
//

#ifndef ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_SCENEMANAGER_HPP
#define ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_SCENEMANAGER_HPP

#include "../../objects/AllObjects.hpp"
#include "glm/glm.hpp"
#include <coroutine>
#include <map>
#include <memory>
#include <string>

class SceneManager {
public:
  using OneTextureObjects = std::vector<std::shared_ptr<BaseObject>>;
  using OneTypeObjects = std::map<std::string, OneTextureObjects>;

  struct SceneManagerProps {};

  struct CameraData {
    glm::mat4 View;
    glm::mat4 Projection;
    glm::mat4 ViewProjection;
  };

  SceneManager(SceneManagerProps Props);

  void addObject(std::shared_ptr<BaseObject> Object);
  void removeObject(const std::string &ObjectName);

  bool goToNextDump();
  OneTypeObjects getNextObjects();
  OneTypeObjects getCurrentObjects() const;
  void resetObjectGetter();

  CameraData getCamera(glm::vec2 WindowSize);

  std::vector<glm::mat4> getTransformations() {
    std::vector<glm::mat4> Result;
    for (auto &[DumpName, MultiTypeObjects] : Objects) {
      for (auto &[Type, OneTypeObjects] : MultiTypeObjects) {
        for (auto &Object : OneTypeObjects) {
          for (auto &ObjectWithTexture : Object.second) {
            Result.push_back(ObjectWithTexture->calculateTransformation());
          }
        }
      }
    }
    return Result;
  }

protected:
  using MultiTypeObjects = std::map<BaseObject::ObjectTypes, OneTypeObjects>;
  std::map<std::string, MultiTypeObjects> Objects;

  std::map<std::string, MultiTypeObjects>::iterator ObjectGetterIt;
  MultiTypeObjects::iterator TypeIt;
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_SCENEMANAGER_HPP