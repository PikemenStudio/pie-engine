//
// Created by anton on 5/22/24.
//

#include "IntroScreen.h"

#include <iostream>
#include <loguru.hpp>

IntroScreen::IntroScreen(sf::RenderWindow* Win)
: Window(Win)
{
  if (!TextFont.loadFromFile("../../game/resources/Fonts/Ostrovsky-Bold.otf"))
  {
    LOG_F(ERROR, "Cannot load font!");
  }

  addLine(L"Меня зовут Вито Скалетта.");
  addLine(L"Используйте стрелки для перемещения.");
  addLine(L"V - снизить/увеличить яркость лампы.");
  addLine(L"E - пройти через проход.");
}

void IntroScreen::addLine(const std::wstring& LineText)
{
  std::unique_ptr<sf::Text> Text = std::make_unique<sf::Text>();
  Text->setFont(TextFont);
  Text->setString(LineText);
  // in pixels, not points!
  Text->setCharacterSize(30);
  const sf::Color TextColor(200, 200, 200);
  Text->setFillColor(TextColor);

  float TextPosX = 10;
  float TextPosY;

  if (Lines.empty())
  {
    TextPosY = 10;
  }
  else
  {
    TextPosY = Lines.back()->getPosition().y +
               Lines.back()->getLineSpacing() +
               Lines.back()->getCharacterSize();
  }

  Text->setPosition(TextPosX, TextPosY);

  Lines.push_back(std::move(Text));
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

void IntroScreen::drawText()
{
  for (const auto& Line : Lines)
  {
    Window->draw(*Line);
  }
}

void IntroScreen::renderScreen()
{
  Window->clear(sf::Color(30, 30, 30));
  drawText();
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
