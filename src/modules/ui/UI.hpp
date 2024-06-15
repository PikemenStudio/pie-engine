//
// Created by FullHat on 09/06/2024.
//

#ifndef ENGINE_SRC_UI_UI_HPP
#define ENGINE_SRC_UI_UI_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "../objects/AllObjects.hpp"

class UI {
public:
  static void draw(uint32_t width, uint32_t height);

  inline static uint32_t height;
  inline static uint32_t width;

  // For list of objects
  inline static std::vector<std::string> Objects{
      "1",
      "2",
      "3",
  };
  inline static std::string CurrentObjectName {
      "1",
  };
  inline static std::shared_ptr<BaseObject> CurrentObject;

protected:
  static void drawList();
  static void drawObjectModification();
};

#endif // ENGINE_SRC_UI_UI_HPP
