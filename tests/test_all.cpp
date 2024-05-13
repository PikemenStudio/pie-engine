//
// Created by FullHat on 29/03/2024.
//

#include <gtest/gtest.h>

#include "graphic_module.hpp"
#include "renderable_scene.hpp"
#include "shader_loader_module.h"
#include "sound_module.hpp"
#include "window_module.hpp"

// Entry point for running tests
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
