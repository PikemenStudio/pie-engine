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
  struct SceneManagerProps {};

  struct ObjectData {
    std::vector<uint8_t> Vertexes;
    glm::vec3 Position;
    std::string DumpName;
    std::string Name;
    BaseObject::ObjectTypes Type;
  };

  struct CameraData {
    glm::mat4 View;
    glm::mat4 Projection;
    glm::mat4 ViewProjection;
  };

  SceneManager(SceneManagerProps Props);

  std::string addObject(std::unique_ptr<BaseObject> Object);
  void addObject(std::string Name, std::unique_ptr<BaseObject> Object);

  std::optional<ObjectData> getNextObject();
  void resetObjectGetter(const std::string &DumpName);

  CameraData getCamera(glm::vec2 WindowSize);

protected:
  using ObjectsDumps = std::map<std::string, std::unique_ptr<BaseObject>>;
  std::map<std::string, ObjectsDumps> Objects;

  std::map<std::string, std::unique_ptr<BaseObject>>::iterator ObjectGetterIt;
  std::string CurrentDumpName;
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_SCENEMANAGER_HPP
