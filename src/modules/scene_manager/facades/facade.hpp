//
// Created by FullHat on 13/05/2024.
//

#ifndef ENGINE_SRC_MODULES_SCENE_MANAGER_FACADES_FACADE_HPP
#define ENGINE_SRC_MODULES_SCENE_MANAGER_FACADES_FACADE_HPP

#include "../../objects/AllObjects.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <concepts>
#include <optional>
#include <utility>
#include <vector>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Define all structures to be used in the facades
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct SceneManagerFacadeStructs {
  template <typename DependencyStructT> struct SceneManagerProps {
    // Add your properties here
    DependencyStructT Dependencies;
  };

  struct CameraData {
    glm::mat4 View;
    glm::mat4 Projection;
    glm::mat4 ViewProjection;
  };

  using OneTextureObjects = std::vector<std::shared_ptr<BaseObject>>;
  using OneTypeObjects = std::map<std::string, OneTextureObjects>;
};

class BaseIterator {
protected:
  using OneTextureObjects = std::vector<std::shared_ptr<BaseObject>>;
  using OneTypeObjects = std::map<std::string, OneTextureObjects>;

public:
  // Constructor that initializes the iterator with a SceneManager instance
  BaseIterator() = default;

  // Virtual destructor
  virtual ~BaseIterator() = default;

  // Dereference operator to access the current OneTypeObjects
  virtual OneTypeObjects get() = 0;
  virtual OneTypeObjects &operator*() = 0;

  // Method to switch to the next shader set and return its name
  virtual std::string_view switchToNextShaderSet() = 0;

  // Method to switch to the next dump and return its name
  virtual std::string_view switchToNextDump() = 0;

  // Method to switch to the next type and return whether the switch was
  // successful
  virtual bool switchToNextType() = 0;

  virtual std::string getCurrentShaderSetName() = 0;
  virtual std::string getCurrentDumpName() = 0;

  virtual bool hasNoMoreObjects() = 0;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Concept to get errors earlier if the Impl is not valid
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename T, typename DependencyStructT>
concept SceneManagerImpl = requires(T Obj) {
  std::is_constructible_v<
      T, SceneManagerFacadeStructs::SceneManagerProps<DependencyStructT>>;
  // Add your requirements here
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Blueprint for the SceneManager Implementation, add functions here
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename DependencyStructT>
concept SceneManagerDependenciesConcept = requires(DependencyStructT Dep) {
  { true };
};

#define SCENE_MANAGER_IMPL(name, DependencyStructConcept)                      \
  template <DependencyStructConcept Dep> class SceneManager##name##Impl {      \
  public:                                                                      \
    using DepType = Dep;                                                       \
                                                                               \
    SceneManager##name##Impl(                                                  \
        SceneManagerFacadeStructs::SceneManagerProps<Dep> &&);                 \
    ~SceneManager##name##Impl();                                               \
                                                                               \
    void addObject(std::shared_ptr<BaseObject> Object);                        \
    void removeObject(const std::string &ObjectName);                          \
                                                                               \
    std::unique_ptr<BaseIterator> begin();                                     \
                                                                               \
    SceneManagerFacadeStructs::CameraData getCamera(glm::vec2 WindowSize);     \
    void setCamera(glm::vec3 From, glm::vec3 To);                              \
                                                                               \
    std::vector<glm::mat4> getTransformations();                               \
                                                                               \
    std::shared_ptr<BaseObject> getObjectByName(const std::string &Name);      \
                                                                               \
    void addPointLight(std::shared_ptr<PointLight> Light);                     \
    std::shared_ptr<PointLight> getPointLight(std::string Name);               \
    std::vector<std::shared_ptr<PointLight>> getPointLights();                 \
                                                                               \
    void checkObjects();                                                       \
                                                                               \
  protected:                                                                   \
    void *Data;                                                                \
  };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Ready for use implementations
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace scene_manager_facades {
struct SceneManagerDependencies {};
SCENE_MANAGER_IMPL(Base, SceneManagerDependenciesConcept)
} // namespace scene_manager_facades

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Facade to be used by the user
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename DependencyStructT,
          SceneManagerImpl<DependencyStructT> Impl =
              scene_manager_facades::SceneManagerBaseImpl<DependencyStructT>>
class SceneManagerFacade {
public:
  using DepType = DependencyStructT;

  SceneManagerFacade(
      SceneManagerFacadeStructs::SceneManagerProps<DependencyStructT> &&Props)
      : ImplInstance(std::move(Props)) {}

public:
  Impl ImplInstance;
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_FACADES_FACADE_HPP
