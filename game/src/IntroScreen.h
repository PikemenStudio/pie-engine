//
// Created by anton on 5/22/24.
//

#ifndef ENGINE_INTROSCREEN_H
#define ENGINE_INTROSCREEN_H

#include <SFML/Graphics.hpp>

#include <memory>
#include <vector>

#include "AbstractTextScreen.h"

class IntroScreen : public AbstractTextScreen
{
public:
  IntroScreen(sf::RenderWindow* Win);

  void init() override;
};

#endif // ENGINE_INTROSCREEN_H
