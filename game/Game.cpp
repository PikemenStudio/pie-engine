//
// Created by anton on 5/13/24.
//

#include "Game.h"
#include "Background.h"
#include "Floor.h"
#include "LightSource.h"
#include "utils.h"

#include <SFML/Graphics.hpp>

#include <loguru.hpp>

#include <iostream>
#include <chrono>

Game::Game()
{
  Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(Width, Height), Title);

  if (!sf::Shader::isAvailable())
  {
    throw std::runtime_error("Shaders are not available!");
  }
}

Game::~Game() = default;

void Game::run()
{
  const std::string FragmentShaderCode = loadTextFile("../../game/shaders/lighting_fragment_shader.glsl");
  sf::Shader LightingShader;
  LightingShader.loadFromMemory(FragmentShaderCode, sf::Shader::Fragment);

  sf::RenderTexture RenderTex;
  RenderTex.create(Width, Height);
  RenderTex.display();
  sf::Sprite ScreenSprite(RenderTex.getTexture());

  Background BG(Width, Height);
  Floor FloorObj(-1, 1, 0.01f);
  LightSource Lantern(0, 0);

  auto BeginTime = std::chrono::high_resolution_clock::now();
  long long FrameCount = 0;

  while (Window->isOpen())
  {
    sf::Event Event;
    while (Window->pollEvent(Event))
    {
      if (Event.type == sf::Event::Closed)
        Window->close();
    }

    Window->clear(sf::Color::Black);

    BG.draw(RenderTex);
    Lantern.draw(RenderTex);
    FloorObj.draw(RenderTex);
    // lighting
//
//    auto LanternPos = Lantern.getPosition();
//    auto LeftTopWorld = screenCoordsToWorld(sf::Vector2f(0, 0));
//    auto CurrScreenPointWorld = LeftTopWorld;
//    for (int X = 0; X < Width; X++)
//    {
//      for (int Y = 0; Y < Height; Y++)
//      {
//        auto Intensity = std::clamp(1.0f / vectorLenSquared(CurrScreenPointWorld - LanternPos), 0.0f, 1.0f);
//      }
//    }

//    Window->draw(ScreenSprite, &LightingShader);
    Window->draw(ScreenSprite);
    Window->display();

    FrameCount++;
  }

  auto TimeDelta = std::chrono::high_resolution_clock::now() - BeginTime;
  auto TimeDeltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(TimeDelta);
  std::cout << "Avg FPS: " << 1000.0 / (static_cast<double>(TimeDeltaMs.count()) / FrameCount) << "\n";
  std::cout << "Total frames: " << FrameCount << "\n";
}