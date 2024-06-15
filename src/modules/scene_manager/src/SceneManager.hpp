//
// Created by FullHat on 14/05/2024.
//

#ifndef ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_SCENEMANAGER_HPP
#define ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_SCENEMANAGER_HPP

#include "../../objects/AllObjects.hpp"
#include "../facades/facade.hpp"
#include "glm/glm.hpp"
#include <coroutine>
#include <map>
#include <memory>
#include <string>

class SceneManager {
public:
  using OneTextureObjects = std::vector<std::shared_ptr<BaseObject>>;
  using OneTypeObjects = std::map<std::string, OneTextureObjects>;

protected:
  using MultiTypeObjects = std::map<BaseObject::ObjectTypes, OneTypeObjects>;
  using MultiDumpObjects = std::map<std::string, MultiTypeObjects>;

public:
  struct SceneManagerProps {};

  struct CameraData {
    glm::mat4 View;
    glm::mat4 Projection;
    glm::mat4 ViewProjection;
  };

  SceneManager(SceneManagerProps Props);

  void addObject(std::shared_ptr<BaseObject> Object);
  void removeObject(const std::string &ObjectName);

  CameraData getCamera(glm::vec2 WindowSize);
  void setCamera(glm::vec3 From, glm::vec3 To);

  std::vector<glm::mat4> getTransformations() {
    std::vector<glm::mat4> Result;
    for (auto &[ShaderSetName, Objects] : MultiShaderSetObjects) {
      for (auto &[DumpName, MultiTypeObjects] : Objects) {
        for (auto &[Type, OneTypeObjects] : MultiTypeObjects) {
          for (auto &Object : OneTypeObjects) {
            for (auto &ObjectWithTexture : Object.second) {
              Result.push_back(ObjectWithTexture->calculateTransformation());
            }
          }
        }
      }
    }
    return Result;
  }

  std::shared_ptr<BaseObject> getObjectByName(const std::string &Name) {
    for (auto &[ShaderSetName, Objects] : MultiShaderSetObjects) {
      for (auto &[DumpName, MultiTypeObjects] : Objects) {
        for (auto &[Type, OneTypeObjects] : MultiTypeObjects) {
          for (auto &Object : OneTypeObjects) {
            for (auto &ObjectWithTexture : Object.second) {
              if (ObjectWithTexture->getName() == Name) {
                return ObjectWithTexture;
              }
            }
          }
        }
      }
    }
    return nullptr;
  }

  class Iterator : BaseIterator {
  public:
    Iterator(SceneManager *Manager) : Manager(Manager) {
      MultipleDumpObjectsIt = Manager->MultiShaderSetObjects.begin();
      MultipleTypeObjectsIt = MultipleDumpObjectsIt->second.begin();
      TypeObjectsIt = MultipleTypeObjectsIt->second.begin();
    }

    virtual OneTypeObjects get() override {
      if (TypeObjectsIt == MultipleTypeObjectsIt->second.end()) {
        throw std::runtime_error("No more objects");
      }
      return TypeObjectsIt->second;
    }

    virtual OneTypeObjects &operator*() override {
      if (TypeObjectsIt == MultipleTypeObjectsIt->second.end()) {
        throw std::runtime_error("No more objects");
      }
      return TypeObjectsIt->second;
    }

    virtual std::string_view switchToNextShaderSet() override {
      // Check if it is end of map
      if (MultipleDumpObjectsIt == Manager->MultiShaderSetObjects.end()) {
        return "";
      }

      MultipleDumpObjectsIt++;

      if (MultipleDumpObjectsIt == Manager->MultiShaderSetObjects.end()) {
        return "";
      }
      MultipleTypeObjectsIt = MultipleDumpObjectsIt->second.begin();
      if (MultipleTypeObjectsIt == MultipleDumpObjectsIt->second.end()) {
        return "";
      }
      TypeObjectsIt = MultipleTypeObjectsIt->second.begin();
      return MultipleDumpObjectsIt->first;
    }

    virtual std::string_view switchToNextDump() override {
      // Check if it is end of map
      if (MultipleTypeObjectsIt == MultipleDumpObjectsIt->second.end()) {
        return "";
      }

      MultipleTypeObjectsIt++;
      if (MultipleTypeObjectsIt == MultipleDumpObjectsIt->second.end()) {
        return "";
      }
      TypeObjectsIt = MultipleTypeObjectsIt->second.begin();
      return MultipleTypeObjectsIt->first;
    }

    virtual bool switchToNextType() override {
      // Check if it is end of map
      if (TypeObjectsIt == MultipleTypeObjectsIt->second.end()) {
        return false;
      }

      TypeObjectsIt++;
      return TypeObjectsIt != MultipleTypeObjectsIt->second.end();
    }

    virtual std::string getCurrentShaderSetName() override {
      return MultipleDumpObjectsIt->first;
    }

    virtual std::string getCurrentDumpName() override {
      return MultipleTypeObjectsIt->first;
    }

    virtual bool hasNoMoreObjects() override {
      if (MultipleDumpObjectsIt == Manager->MultiShaderSetObjects.end()) {
        return true;
      }
      if (MultipleTypeObjectsIt == MultipleDumpObjectsIt->second.end()) {
        return true;
      }
      if (TypeObjectsIt == MultipleTypeObjectsIt->second.end()) {
        return true;
      }
      if (TypeObjectsIt->second.empty()) {
        return true;
      }
      return false;
    }

  protected:
    SceneManager *Manager;

    std::map<std::string, MultiDumpObjects>::iterator MultipleDumpObjectsIt;
    std::map<std::string, MultiTypeObjects>::iterator MultipleTypeObjectsIt;
    MultiTypeObjects::iterator TypeObjectsIt;
  };

  std::unique_ptr<BaseIterator> begin() {
    return std::unique_ptr<BaseIterator>((BaseIterator *)new Iterator(this));
  }

protected:
  std::map<std::string, MultiDumpObjects> MultiShaderSetObjects;

  std::map<std::string, MultiTypeObjects>::iterator ObjectGetterIt;
  glm::vec3 From = {0.0f, 10.0f, -10.0f};
  glm::vec3 To = {0.0f, 0.0f, 0.0f};
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_SCENEMANAGER_HPP
