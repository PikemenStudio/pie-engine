//
// Created by anton on 5/13/24.
//

#ifndef ENGINE_CAVESCENE_H
#define ENGINE_CAVESCENE_H

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

class CaveScene {
public:
  CaveScene(sf::RenderWindow* Win);
  ~CaveScene();

  void run();

private:
  void initKeyboard();
  void initGameObjects();

  void positionPlayer();

  void createPassage(Tunnel* T1, Tunnel* T2, float XCoord);
  void generateTunnels(const sf::Vector2f& PlSize);
  void transferToTunnel(Tunnel* T1, Tunnel* T2, Passage* Pass);

  void generateRats();
  void generateRatsInTunnel(Tunnel* T);

  void handleUserInput();
  void processLogic(float FrameDrawingTimeMs);

  void drawHUD();
  void renderScene();

//  std::unique_ptr<sf::RenderWindow> Window;
  sf::RenderWindow* Window;
  std::unique_ptr<sf::Shader> PostprocessingShader;
  std::unique_ptr<sf::RenderTexture> RenderTex;
  std::unique_ptr<sf::Sprite> ScreenSprite;
  std::unique_ptr<sf::Font> TextFont;

  // game objects
  std::vector<std::unique_ptr<Tunnel>> Tunnels;
  std::vector<std::unique_ptr<Passage>> Passages;
  std::vector<std::unique_ptr<DimmingTransition>> Transitions;
  std::vector<std::unique_ptr<LightSource>> LightSources;
  std::unique_ptr<Background> Backgr;
  std::unique_ptr<Player> PlayerObj;
  std::vector<std::unique_ptr<Rat>> Rats;
  std::unique_ptr<WorldWindow> WorldWindowObj;

  std::vector<SolidObject*> SolidObjects;
  std::vector<Interactable*> Interactables;
  // maybe TODO
//  std::vector<Drawable*> Drawables;

  std::map<sf::Keyboard::Key, bool> Key2IsPressed;

  float BackgrIntensity = 1.0f;

  const std::string Title = "CaveScene";
};

#endif // ENGINE_CAVESCENE_H