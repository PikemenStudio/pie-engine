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

  enum class ObjectTypes {
    TRIANGLE,
  };

  struct ObjectData {
    std::vector<uint8_t> Vertexes;
    glm::vec3 Position;

    ObjectTypes Type;
    struct TransformationStruct {
      glm::mat4 Transformation;
    };
    TransformationStruct calculateTransformation() {
      glm::mat4 Result = glm::translate(glm::mat4(1.0f), Position);
      return {.Transformation = Result};
    }
  };
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
    std::string addObject(std::unique_ptr<BaseObject> Object);                 \
    void addObject(std::string Name, std::unique_ptr<BaseObject> Object);      \
                                                                               \
    std::optional<SceneManagerFacadeStructs::ObjectData> getNextObject();      \
    void resetObjectGetter();                                                  \
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
