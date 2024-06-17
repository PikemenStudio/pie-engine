//
// Created by FullHat on 16/06/2024.
//

#ifndef ENGINE_SRC_MODULES_OBJECTS_POINTLIGHT_HPP
#define ENGINE_SRC_MODULES_OBJECTS_POINTLIGHT_HPP

#include "glm/glm.hpp"
#include <utility>
#include <string>

class PointLight {
public:
  struct PointLightProps {
    glm::vec4 position;
    glm::vec4 color;
    glm::float32_t intensity;

    std::string Name;
  };

  PointLight(PointLightProps &&InitProps) : Props(std::move(InitProps)) {}

  void setProps(PointLightProps &&NewProps) {
    this->Props = std::move(NewProps);
  }
  void setPosition(glm::vec4 Position) { Props.position = Position; }
  void setColor(glm::vec4 Color) { Props.color = Color; }
  void setIntensity(glm::float32_t Intensity) { Props.intensity = Intensity; }

  [[nodiscard]] PointLightProps getProps() const { return Props; }

protected:
  PointLightProps Props;
};

#endif // ENGINE_SRC_MODULES_OBJECTS_POINTLIGHT_HPP
