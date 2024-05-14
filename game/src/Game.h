//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_GAME_H
#define ENGINE_GAME_H

#include <map>
#include <memory>
#include <string>

#include <SFML/Graphics.hpp>

class LightSource;
class Background;
class Floor;
class WorldWindow;

class Game
{
public:
  Game();
  ~Game();

  void run();

private:
  void handleUserInput();
  void processLogic(float FrameDrawingTimeMs);
  void renderScene();

  std::unique_ptr<sf::RenderWindow> Window;
  std::unique_ptr<sf::Shader> PostprocessingShader;
  std::unique_ptr<sf::RenderTexture> RenderTex;
  std::unique_ptr<sf::Sprite> ScreenSprite;

  // game objects
  std::unique_ptr<LightSource> Lantern;
  std::unique_ptr<Floor> FloorObj;
  std::unique_ptr<Background> Backgr;
  std::unique_ptr<WorldWindow> WorldWindowObj;

  const std::string Title = "Game";
  std::map<std::string, bool> Key2IsPressed;
};

#endif // ENGINE_GAME_H
