#include "gtest/gtest.h"

#include "Core/Engine.hpp"

TEST(system, json) {}

int main(int argc, char **argv) {
  auto engine = Engine();

  engine.RunMainCycle();

  /*::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();*/
}
