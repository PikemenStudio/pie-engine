//
// Created by anton on 5/22/24.
//

#include "IntroScreen.h"

#include <iostream>

IntroScreen::IntroScreen(sf::RenderWindow* Win)
: Window(Win)
{
}

void IntroScreen::handleUserInput()
{
  sf::Event Event;
  while (Window->pollEvent(Event))
  {
    if (Event.type == sf::Event::Closed)
      Window->close();
    if (Event.type == sf::Event::KeyPressed)
    {
      IsRunning = false;
    }
  }
}

void IntroScreen::renderScreen()
{
  Window->clear(sf::Color(40, 40, 40));
  Window->display();
}

void IntroScreen::run()
{
  while (Window->isOpen() && IsRunning)
  {
    handleUserInput();
    renderScreen();
  }
}
