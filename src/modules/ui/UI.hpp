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

  inline static std::vector<std::string> Textures;
  inline static int CurrentTextureIndex;
  inline static std::function<void()> TextureUpdated;

  inline static std::string objectName;
  inline static std::function<void(std::string, std::string)> ObjectNameUpdated;

  inline static std::function<void()> DeleteObject;

  inline static std::vector<std::string> ObjectTypes;
  inline static std::vector<std::string> AllTextures;
  inline static int CreationObjectTypeIndex;
  inline static int CreationObjectTextureIndex;
  inline static std::string CreationObjectName;
  inline static std::string FilePath;
  inline static std::vector<std::string> ShaderSetsNames;
  inline static int CreationObjectShaderSetIndex;
  inline static std::function<void(std::shared_ptr<BaseObject>)> CreationObject;

protected:
  static void drawList();
  static void drawObjectModification();
  static void drawObjectCreationWindow();

  inline static ObjectModificationType ObjectModification;
};

#endif // ENGINE_SRC_UI_UI_HPP
