//
// Created by anton on 5/22/24.
//

#ifndef ENGINE_INTROSCREEN_H
#define ENGINE_INTROSCREEN_H

#include <SFML/Graphics.hpp>

#include <memory>
#include <vector>

class IntroScreen
{
public:
  IntroScreen(sf::RenderWindow* Win);

  void addLine(const std::wstring& LineText);

  void handleUserInput();
  void drawText();
  void renderScreen();
  void run();

private:
  sf::RenderWindow* Window;

  sf::Font TextFont;
  bool IsRunning = true;

  std::vector<std::unique_ptr<sf::Text>> Lines;
};

#endif // ENGINE_INTROSCREEN_H
