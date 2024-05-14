//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_GAME_H
#define ENGINE_GAME_H

#include <map>
#include <memory>
#include <string>

namespace sf
{
class RenderWindow;
class Shader;
class RenderTexture;
class Sprite;
} // namespace sf

class LightSource;

class Game
{
public:
  Game();
  ~Game();

  void run();

private:
  void handleUserInput();
  void processLogic(float FrameDrawingTimeMs);

  std::unique_ptr<sf::RenderWindow> Window;
//  std::unique_ptr<sf::Shader> LightingShader;
//  std::unique_ptr<sf::RenderTexture> RenderTex;
//  std::unique_ptr<sf::Sprite> ScreenSprite;

  // game objects
  std::unique_ptr<LightSource> Lantern;

  const std::string Title = "Game";
  std::map<std::string, bool> Key2IsPressed;
};

#endif // ENGINE_GAME_H
