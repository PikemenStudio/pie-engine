//
// Created by anton on 5/24/24.
//

#ifndef ENGINE_ABSTRACTTEXTSCREEN_H
#define ENGINE_ABSTRACTTEXTSCREEN_H

#include <SFML/Graphics.hpp>

class AbstractTextScreen
{
public:
  AbstractTextScreen(sf::RenderWindow* Win);

  virtual void init() = 0;
  virtual void run();

protected:
  virtual void handleUserInput();
  virtual void drawText();
  virtual void renderScreen();
  virtual void addLine(const std::wstring& LineText);

  sf::RenderWindow* Window;

  sf::Font TextFont;
  bool IsRunning = true;

  std::vector<std::unique_ptr<sf::Text>> Lines;
};

#endif // ENGINE_ABSTRACTTEXTSCREEN_H
