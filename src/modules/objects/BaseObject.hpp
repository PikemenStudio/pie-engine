//
// Created by FullHat on 14/05/2024.
//

#ifndef ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_BASEOBJECT_HPP
#define ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_BASEOBJECT_HPP

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

class BaseObject {
public:
  BaseObject() = default;
  BaseObject(BaseObject &&) = default;
  BaseObject(BaseObject &) = default;
  virtual ~BaseObject() = default;

  virtual std::vector<float> getVertices() = 0;
  virtual std::vector<float> getColors() { return {}; }
  virtual std::vector<float> getTextureCoords() { return {}; }

  enum class ObjectTypes {
    TRIANGLE,
    SQUARE,
    ACTOR,
  };

  static std::string toString(ObjectTypes Type) {
    switch (Type) {
    case ObjectTypes::TRIANGLE:
      return "Triangle";
    case ObjectTypes::SQUARE:
      return "Square";
    case ObjectTypes::ACTOR:
      return "Actor";
    }
  }

  // Object data
  virtual ObjectTypes getType() = 0;
  virtual std::string getDumpName() { return DumpName; }
  virtual std::string getName() { return Name; }
  virtual std::string getTextureName() { return TextureName; }
  virtual std::string getShaderSetName() { return ShaderSetName; }

  virtual void setName(std::string NewName) { this->Name = NewName; }
  virtual void setTextureName(std::string NewTextureName) {
    this->TextureName = NewTextureName;
  }
  virtual void setDumpName(std::string NewDumpName) {
    this->DumpName = NewDumpName;
  }
  virtual void setShaderSetName(std::string NewShaderSetName) {
    this->ShaderSetName = NewShaderSetName;
  }

  // Modifications
  virtual void moveBy(glm::vec3 Vector) { this->Position = Vector; }
  virtual void rotateBy(glm::vec3 Axis, float Angle) {
    Rotation = glm::rotate(glm::mat4(1.0f), glm::radians(Angle), Axis);
  }
  virtual void scaleBy(glm::vec3 Vector) { this->Scale = Vector; }

  virtual glm::vec3 getPosition() { return Position; }
  virtual glm::mat4 getRotation() { return Rotation; }
  virtual glm::vec3 getScale() { return Scale; }

  virtual glm::mat4 calculateTransformation() = 0;

  virtual void update() = 0;

protected:
  std::string Name;
  std::string TextureName;
  std::string DumpName;
  std::string ShaderSetName = "default";

  glm::vec3 Position = {0.0f, 0.0f, 0.0f};
  glm::mat4 Rotation = glm::mat4(1.0f);
  glm::vec3 Scale = {1.0f, 1.0f, 1.0f};
};

#endif // ENGINE_SRC_MODULES_SCENE_MANAGER_SRC_OBJECTS_BASEOBJECT_HPP
