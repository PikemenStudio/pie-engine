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
  };

  SceneManager(SceneManagerProps Props);

  std::string addObject(std::unique_ptr<BaseObject> Object);
  void addObject(std::string Name, std::unique_ptr<BaseObject> Object);

  std::optional<ObjectData> getNextObject();
  void resetObjectGetter();

protected:
  std::map<std::string, std::unique_ptr<BaseObject>> Objects;

  std::map<std::string, std::unique_ptr<BaseObject>>::iterator ObjectGetterIt;
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_SCENEMANAGER_HPP
