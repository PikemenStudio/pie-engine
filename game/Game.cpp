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

#include <chrono>
#include <iostream>
#include <map>

Game::Game()
{
  Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(ScreenWidth, ScreenHeight),
                                              Title);
//                                              sf::Style::Fullscreen);
  Window->setFramerateLimit(100);

  if (!sf::Shader::isAvailable())
  {
    throw std::runtime_error("Shaders are not available!");
  }
}

Game::~Game() = default;

void Game::run()
{
  sf::Shader LightingShader;
  LightingShader.loadFromFile("../../game/shaders/lighting_vertex_shader.glsl",
                              "../../game/shaders/lighting_fragment_shader.glsl");
  LightingShader.setUniform("texture", sf::Shader::CurrentTexture); // always use current texture

  sf::RenderTexture RenderTex;
  RenderTex.create(ScreenWidth, ScreenHeight);
  RenderTex.display();
  sf::Sprite ScreenSprite(RenderTex.getTexture());

  Background BG(ScreenWidth, ScreenHeight);
  Floor FloorObj(-5, 5, 0.01f);
  LightSource Lantern(0, 0);

  std::map<std::string, bool> Key2IsPressed;
  Key2IsPressed["left"] = false;
  Key2IsPressed["right"] = false;

  auto BeginTime = std::chrono::high_resolution_clock::now();
  long long FrameCount = 0;
  float FrameDrawingTimeMs = 0;

  while (Window->isOpen())
  {
    auto BeginFrameTime = std::chrono::high_resolution_clock::now();

    // user input
    sf::Event Event;
    while (Window->pollEvent(Event))
    {
      if (Event.type == sf::Event::Closed)
        Window->close();
      if (Event.type == sf::Event::KeyPressed)
      {
        if (Event.key.code == sf::Keyboard::Left)
          Key2IsPressed["left"] = true;
        else if (Event.key.code == sf::Keyboard::Right)
          Key2IsPressed["right"] = true;
        else if (Event.key.code == sf::Keyboard::Escape)
          Window->close();
      }
      else if (Event.type == sf::Event::KeyReleased)
      {
        if (Event.key.code == sf::Keyboard::Left)
          Key2IsPressed["left"] = false;
        else if (Event.key.code == sf::Keyboard::Right)
          Key2IsPressed["right"] = false;
      }
    }

    // game logic
    float LanternFrameSpeed = FrameDrawingTimeMs / 1000 * 1.0;
    sf::Vector2f LanternDxDy;
    if (Key2IsPressed["left"])  LanternDxDy.x -= LanternFrameSpeed;
    if (Key2IsPressed["right"]) LanternDxDy.x += LanternFrameSpeed;

    Lantern.setPosition(Lantern.getPosition() + LanternDxDy);

    // drawing
    sf::FloatRect WorldWindow;
    WorldWindow.width = 3.0f;
    WorldWindow.height = 2.0f;
    WorldWindow.left = (Lantern.getPosition().x - WorldWindow.width / 2);

    if (WorldWindow.left < FloorObj.getStartX())
      WorldWindow.left = FloorObj.getStartX();
    else if (WorldWindow.left + WorldWindow.width > FloorObj.getEndX())
      WorldWindow.left = FloorObj.getEndX() - WorldWindow.width;

    WorldWindow.top = WorldWindow.height / 2;

    Window->clear(sf::Color::Black);

    BG.draw(RenderTex, WorldWindow);
    Lantern.draw(RenderTex, WorldWindow);
    FloorObj.draw(RenderTex, WorldWindow);

    auto WorldWinCenter = sf::Vector2f(WorldWindow.left, WorldWindow.top) +
                                        sf::Vector2f(WorldWindow.width/2, -WorldWindow.height/2);

    LightingShader.setUniform("world_window_center", WorldWinCenter);
    LightingShader.setUniform(
        "world_window_dimensions",
        sf::Vector2f(WorldWindow.width, WorldWindow.height));
    LightingShader.setUniform("world_light_pos", Lantern.getPosition());

    Window->draw(ScreenSprite, &LightingShader);
//    Window->draw(ScreenSprite);
    Window->display();

    FrameCount++;

    auto FrameTimeDelta = std::chrono::high_resolution_clock::now() - BeginFrameTime;
    FrameDrawingTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(FrameTimeDelta).count();
  }

  auto TimeDelta = std::chrono::high_resolution_clock::now() - BeginTime;
  auto TimeDeltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(TimeDelta);
  std::cout << "Avg FPS: " << 1000.0 / (static_cast<double>(TimeDeltaMs.count()) / FrameCount) << "\n";
  std::cout << "Total frames: " << FrameCount << "\n";
}