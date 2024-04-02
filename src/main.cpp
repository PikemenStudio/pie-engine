#include <iostream>
//
// Created by FullHat on 28/03/2024.
//
#include <loguru.hpp>

int main(int argc, char *argv[]) {
  loguru::init(argc, argv);
  loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);

  std::cout << "Hello world" << std::endl;
  return 0;
}