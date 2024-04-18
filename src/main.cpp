#include <iostream>
//
// Created by FullHat on 28/03/2024.
//
#include <loguru.hpp>

#include "modules/graphics/facades/facade.hpp"
#include "modules/windows/facades/facade.hpp"

int main(int Argc, char *Argv[]) {
  loguru::init(Argc, Argv);
  loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);
  
  return 0;
}