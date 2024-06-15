//
// Created by FullHat on 09/06/2024.
//

#ifndef ENGINE_SRC_UI_UI_HPP
#define ENGINE_SRC_UI_UI_HPP

#include "../objects/AllObjects.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class UI {
public:
  static void draw(uint32_t width, uint32_t height);

  inline static uint32_t height;
  inline static uint32_t width;

  // For list of objects
  inline static std::vector<std::string> Objects;
  inline static std::string CurrentObjectName;
  inline static std::shared_ptr<BaseObject> CurrentObject;

  struct ObjectModificationType {
    glm::vec3 Position;
    glm::vec3 Rotation;
    glm::vec3 Scale;
  };
  using OptionalObjectModificationType = std::optional<ObjectModificationType>;

  static OptionalObjectModificationType getCurrentObjectModification();
  static void
  setCurrentObjectModification(ObjectModificationType NewObjectModification);
  inline static std::function<void()> CurrentObjectUpdated;

protected:
  static void drawList();
  static void drawObjectModification();

  inline static ObjectModificationType ObjectModification;
};

#endif // ENGINE_SRC_UI_UI_HPP
