//
// Created by anton on 5/22/24.
//

#ifndef ENGINE_INTROSCREEN_H
#define ENGINE_INTROSCREEN_H

#include <SFML/Graphics.hpp>

class IntroScreen
{
public:
  IntroScreen(sf::RenderWindow* Win);

  void handleUserInput();
  void renderScreen();
  void run();

private:
  sf::RenderWindow* Window;
  bool IsRunning = true;
};

#endif // ENGINE_INTROSCREEN_H
