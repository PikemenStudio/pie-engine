#include <iostream>
//
// Created by FullHat on 28/03/2024.
//
#include <loguru.hpp>

#include "modules/graphics/facades/facade.hpp"
#include "modules/windows/facades/facade.hpp"
#include "modules/shader_loader/facades/facade.hpp"

int main(int Argc, char *Argv[]) {
  loguru::init(Argc, Argv);
  // clang-format off
  loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);
  // clang-format on

  return 0;
}