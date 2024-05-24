//
// Created by anton on 5/22/24.
//

#include "IntroScreen.h"

#include <iostream>
#include <loguru.hpp>

IntroScreen::IntroScreen(sf::RenderWindow* Win) : AbstractTextScreen(Win)
{
}

void IntroScreen::init()
{
  addLine(L"Меня зовут Вито Скалетта. Я родился на Сицилии в 25 году.");
  addLine(L"Я мало что помню о тех временах, разве что жизнь была тяжёлой.");
  addLine(L"\t");
  addLine(L"Используйте стрелки для перемещения.");
  addLine(L"V - снизить/увеличить яркость лампы.");
  addLine(L"E - пройти через проход.");
  addLine(L"Ваша задача - найти выход.");
  addLine(L"Нажмите любую клавишу...");
}
