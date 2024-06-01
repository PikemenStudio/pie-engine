//
// Created by anton on 5/12/24.
//

#include <loguru.hpp>

#include <sound/facade/facade.hpp>

#include "CaveScene.h"
#include "IntroScreen.h"
#include "utils.h"

int main(int Argc, char** Argv)
{
  loguru::init(Argc, Argv);
  // clang-format off
  loguru::add_file("game.log", loguru::Append, loguru::Verbosity_MAX);
  // clang-format on

  srand(time(nullptr));

  sf::RenderWindow Window(
      sf::VideoMode(ScreenWidth, ScreenHeight),
      "");
  //                                              sf::Style::Fullscreen);
  Window.setFramerateLimit(100);

  IntroScreen Intro(&Window);
  Intro.init();
  Intro.run();

  CaveScene Cave(&Window);
  Cave.run();

  return 0;
}