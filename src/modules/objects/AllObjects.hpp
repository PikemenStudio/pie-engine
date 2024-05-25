//
// Created by FullHat on 14/05/2024.
//

#ifndef ENGINE_SRC_MODULES_OBJECTS_ALLOBJECTS_HPP
#define ENGINE_SRC_MODULES_OBJECTS_ALLOBJECTS_HPP

#include "BaseObject.hpp"
#include "Square.hpp"
#include "Triangle.hpp"
#include "Actor.hpp"
#include <map>

struct ObjectData {
  std::vector<float> Vertexes;
  std::vector<float> Colors;
  std::vector<float> TexCoords;
};

static std::map<BaseObject::ObjectTypes, ObjectData> DefaultData{
    {BaseObject::ObjectTypes::TRIANGLE,
     {{0.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f},
      {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
      {0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}}},
    {BaseObject::ObjectTypes::SQUARE,
     {{{-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f,
        0.5f, 0.0f}},
      {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
       1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
      {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
       1.0f}}}};

#endif // ENGINE_SRC_MODULES_OBJECTS_ALLOBJECTS_HPP
