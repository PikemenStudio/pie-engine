//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_GAME_H
#define ENGINE_GAME_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

class LightSource;
class Background;
class Tunnel;
class WorldWindow;
class Player;
class SolidObject;
class Passage;
class Drawable;
class Interactable;
class DimmingTransition;
class Rat;

class Game
{
public:
  Game();
  ~Game();

  void run();

private:
  void initKeyboard();
  void initGameObjects();

  void positionPlayer();

  void createPassage(Tunnel* T1, Tunnel* T2, float XCoord);
  void generateTunnels(const sf::Vector2f& PlSize);
  void transferToTunnel(Tunnel* T1, Tunnel* T2, Passage* Pass);

  void generateRats();

  void handleUserInput();
  void processLogic(float FrameDrawingTimeMs);

  void drawHUD();
  void renderScene();

  std::unique_ptr<sf::RenderWindow> Window;
  std::unique_ptr<sf::Shader> PostprocessingShader;
  std::unique_ptr<sf::RenderTexture> RenderTex;
  std::unique_ptr<sf::Sprite> ScreenSprite;
  std::unique_ptr<sf::Font> TextFont;

  // game objects
  std::vector<std::unique_ptr<Tunnel>> Tunnels;
  std::vector<std::unique_ptr<Passage>> Passages;
  std::vector<std::unique_ptr<DimmingTransition>> Transitions;
  std::unique_ptr<Background> Backgr;
  std::unique_ptr<Player> PlayerObj;
  std::unique_ptr<LightSource> Lantern;
  std::vector<std::unique_ptr<Rat>> Rats;
  std::unique_ptr<WorldWindow> WorldWindowObj;

  std::vector<SolidObject*> SolidObjects;
  std::vector<Interactable*> Interactables;
  // maybe TODO
//  std::vector<Drawable*> Drawables;

  std::map<sf::Keyboard::Key, bool> Key2IsPressed;

  float BackgrIntensity = 1.0f;

  const std::string Title = "Game";
};

#endif // ENGINE_GAME_H
