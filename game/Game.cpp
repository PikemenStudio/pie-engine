//
// Created by anton on 5/13/24.
//

#include "Game.h"
#include "Floor.h"

#include <SFML/Graphics.hpp>

Game::Game()
{
  Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(Width, Height), Title);
}

Game::~Game() = default;

void Game::run()
{
  Floor FloorObj(-1, 1, 0.01f);

  while (Window->isOpen())
  {
    sf::Event Event;
    while (Window->pollEvent(Event))
    {
      if (Event.type == sf::Event::Closed)
        Window->close();
    }

    Window->clear(sf::Color::White);

    FloorObj.draw(*Window);

    Window->display();
  }
}