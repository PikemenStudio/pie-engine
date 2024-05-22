//
// Created by anton on 5/13/24.
//

#include "Game.h"
#include "Background.h"
#include "DimmingTransition.h"
#include "LightSource.h"
#include "Passage.h"
#include "Player.h"
#include "Rat.h"
#include "Tunnel.h"
#include "WorldWindow.h"

#include <SFML/Graphics.hpp>

#include <loguru.hpp>

#include <chrono>
#include <iostream>

Game::Game()
{
  // objects for rendering
  Window = std::make_unique<sf::RenderWindow>(sf::VideoMode(ScreenWidth, ScreenHeight),
                                              Title);
//                                              sf::Style::Fullscreen);
  Window->setFramerateLimit(100);
  RenderTex = std::make_unique<sf::RenderTexture>();
  RenderTex->create(ScreenWidth, ScreenHeight);
  RenderTex->display();
  ScreenSprite = std::make_unique<sf::Sprite>(RenderTex->getTexture());
  TextFont = std::make_unique<sf::Font>();
//  if (!TextFont->loadFromFile("../../game/resources/Fonts/Ubuntu-B.ttf"))
  if (!TextFont->loadFromFile("../../game/resources/Fonts/Ostrovsky-Bold.otf"))
  {
    LOG_F(ERROR, "Cannot load font!");
  }

  initKeyboard();

  initGameObjects();

  // shaders
  if (!sf::Shader::isAvailable())
    throw std::runtime_error("Shaders are not available!");
  PostprocessingShader = std::make_unique<sf::Shader>();
  PostprocessingShader->loadFromFile("../../game/shaders/lighting_vertex_shader.glsl",
                              "../../game/shaders/lighting_fragment_shader.glsl");
  // always use current texture
  // (the texture of the current object being drawn)
  PostprocessingShader->setUniform("texture", sf::Shader::CurrentTexture);
}

Game::~Game() = default;

void Game::initKeyboard()
{
  Key2IsPressed[sf::Keyboard::Left] = false;
  Key2IsPressed[sf::Keyboard::Right] = false;
  Key2IsPressed[sf::Keyboard::Up] = false;
  Key2IsPressed[sf::Keyboard::Down] = false;
}

void Game::initGameObjects()
{
  Lantern = std::make_unique<LightSource>(0, 0, 0.4f);
  Backgr = std::make_unique<Background>(ScreenWidth, ScreenHeight);

  const sf::Vector2f PlSize = {0.05f, 0.3f};
  PlayerObj = std::make_unique<Player>(
      sf::Vector2f(0, 0), PlSize,
      Lantern.get(), nullptr);
  generateTunnels(PlSize);

  PlayerObj->setCurrTunnel(Tunnels.back().get());
  positionPlayer();

  generateRats();

  SolidObjects.push_back(PlayerObj.get());
  for (const auto& T : Tunnels)
    SolidObjects.push_back(T.get());
  for (const auto& R : Rats)
    SolidObjects.push_back(R.get());
  for (const auto& P : Passages)
    Interactables.push_back(P.get());
}

void Game::positionPlayer()
{
  do
  {
    float XCoord = PlayerObj->getCurrTunnel()->getStartX() + WorldWindowObj->getSize().x / 10 +
                   static_cast<float>(rand() % 1000) / 1000 * WorldWindowObj->getSize().x / 2;
    PlayerObj->setPosition({XCoord, -0.25});
  } while (PlayerObj->isCollision(PlayerObj->getCurrTunnel()));
}

void Game::createPassage(Tunnel* T1, Tunnel* T2, float XCoord)
{
  auto Pass = std::make_unique<Passage>(T1, T2, PlayerObj.get(), XCoord);
  Transitions.push_back(
      std::make_unique<DimmingTransition>(
          BackgrIntensity,
          [this, T1, T2, &Pass](){
            transferToTunnel(T1, T2, Pass.get());
          }));
  Passages.push_back(std::move(Pass));
}

void Game::generateTunnels(const sf::Vector2f& PlSize)
{
  constexpr float MinTunnelLen = 5.0f;
  constexpr float MaxTunnelLen = 10.0f;
  constexpr float StepX = 0.01f;

  float CurrTunnelLen = MinTunnelLen + rand01() * (MaxTunnelLen - MinTunnelLen);
  float StartX = -CurrTunnelLen / 2;
  float EndX = CurrTunnelLen / 2;
  bool GenPassageInRight = rand01() > 0.5;

  LOG_F(INFO, "GenPassageInRight = %d", GenPassageInRight);

  auto T = std::make_unique<Tunnel>(
      StartX, EndX, StepX,
      PlSize.x, PlSize.y,
      !GenPassageInRight, GenPassageInRight);
  T->setVisible(false);
  Tunnels.push_back(std::move(T));
//  PrevTunnel = T.get();

  int NumTunnels = randIntInRange(5, 10);
//  int NumTunnels = 1;

  for (int I = 0; I < NumTunnels; I++)
  {
    float PassageX;
    if (GenPassageInRight)
      PassageX = randInRange(EndX - CurrTunnelLen * 0.2f, EndX - CurrTunnelLen * 0.1f);
    else
      PassageX = randInRange(StartX + CurrTunnelLen * 0.1f, StartX + CurrTunnelLen * 0.2f);

    CurrTunnelLen = randInRange(MinTunnelLen, MaxTunnelLen);
    StartX = PassageX - randInRange(0.1, 0.9) * CurrTunnelLen;
    EndX = StartX + CurrTunnelLen;

    GenPassageInRight = PassageX - StartX < CurrTunnelLen / 2;

    T = std::make_unique<Tunnel>(StartX, EndX, StepX,
                                 PlSize.x, PlSize.y,
                                 true, true);
    T->setVisible(false);
    Tunnels.push_back(std::move(T));

    createPassage(Tunnels[Tunnels.size() - 2].get(),
                  Tunnels[Tunnels.size() - 1].get(),
                  PassageX);
  }

  // Finish

  Tunnel* LastTunnel = Tunnels.back().get();
  LastTunnel->setVisible(true);

  for (int I = 0; I < Passages.size(); I++)
    Passages[I]->setTransition(Transitions[I].get());
  WorldWindowObj = std::make_unique<WorldWindow>(
      sf::Vector2f(0, 0), sf::Vector2f(3, 2),
      LastTunnel->getStartX(), LastTunnel->getEndX());
}

void Game::transferToTunnel(Tunnel* T1, Tunnel* T2, Passage* Pass)
{
  Tunnel* From = PlayerObj->getCurrTunnel() == T1 ? T1 : T2;
  Tunnel* To = From == T1 ? T2 : T1;

  From->setVisible(false);
  To->setVisible(true);

  for (auto& R : Rats)
  {
    if (R->getCurrTunnel() == From)
      R->setVisible(false);
    else if (R->getCurrTunnel() == To)
      R->setVisible(true);
  }

  WorldWindowObj->setSceneBorders(To->getStartX(), To->getEndX());
  PlayerObj->moveToTunnel(To);
}

void Game::generateRats()
{
  int RatsCount = 1;

  Tunnel* CurrTunnel = PlayerObj->getCurrTunnel();
  float CurrTunnelLen = CurrTunnel->getEndX() - CurrTunnel->getStartX();

  for (int I = 0; I < RatsCount; I++)
  {
    float RatXCoord = randInRange(0.1, 0.9) * CurrTunnelLen + CurrTunnel->getStartX();
    auto NewRat = std::make_unique<Rat>(
        sf::Vector2f(0.0f, 0.0f),
        CurrTunnel);
    NewRat->setPosition(
        sf::Vector2f(
            RatXCoord,
            CurrTunnel->getFloorYCoord(RatXCoord) + NewRat->getSize().y * 3));
    Rats.push_back(std::move(NewRat));
  }
}

void Game::handleUserInput()
{
  sf::Event Event;
  while (Window->pollEvent(Event))
  {
    if (Event.type == sf::Event::Closed)
      Window->close();
    if (Event.type == sf::Event::KeyPressed)
    {
      if (Event.key.code == sf::Keyboard::Escape)
        Window->close();
      else if (Event.key.code == sf::Keyboard::E)
      {
        // Interact
        for (auto& Inter : Interactables)
        {
          if (Inter->isInInteractZone(PlayerObj.get()))
            Inter->runInteraction();
        }
      }
      else if (Event.key.code == sf::Keyboard::V)
      {
        if (Lantern->getBaseIntensity() > 0.45f)
        {
          Lantern->setBaseIntensity(0.4f);
        }
        else
        {
          Lantern->setBaseIntensity(0.5f);
        }
      }
      else
        Key2IsPressed[Event.key.code] = true;
    }
    else if (Event.type == sf::Event::KeyReleased)
    {
      Key2IsPressed[Event.key.code] = false;
    }
  }
}

void Game::processLogic(float FrameDrawingTimeMs)
{
  PlayerObj->update(Key2IsPressed, FrameDrawingTimeMs, SolidObjects);
  Lantern->update();

  for (const auto& R : Rats)
    R->update(FrameDrawingTimeMs, SolidObjects, PlayerObj.get());
  for (const auto& T : Transitions)
    T->update();

  WorldWindowObj->updateByPlayerPos(PlayerObj->getPosition());
}

void Game::drawHUD()
{
  sf::Text HealthText, OilText, StaminaText;

  HealthText.setFont(*TextFont.get());
  OilText.setFont(*TextFont.get());
  StaminaText.setFont(*TextFont.get());

  HealthText.setString(L"Здоровье: " +
                       std::to_wstring(static_cast<int>(PlayerObj->getHealth() * 100)));
  OilText.setString(L"Масло: " +
                       std::to_wstring(static_cast<int>(PlayerObj->getOil() * 100)));
  StaminaText.setString(L"Силы: " +
                       std::to_wstring(static_cast<int>(PlayerObj->getStamina() * 100)));

  // in pixels, not points!
  constexpr float WorldCharSize = 0.071f;
  HealthText.setCharacterSize(
      worldDeltaToScreen(sf::Vector2f(WorldCharSize, WorldCharSize),
                         *WorldWindowObj).x);
  OilText.setCharacterSize(
      worldDeltaToScreen(sf::Vector2f(WorldCharSize, WorldCharSize),
                         *WorldWindowObj).x);
  StaminaText.setCharacterSize(
      worldDeltaToScreen(sf::Vector2f(WorldCharSize, WorldCharSize),
                         *WorldWindowObj).x);

  const sf::Color TextColor(200, 200, 200);
  HealthText.setFillColor(TextColor);
  OilText.setFillColor(TextColor);
  StaminaText.setFillColor(TextColor);

  constexpr float XCoord = 0.05f;

  HealthText.setPosition(worldDeltaToScreen({XCoord, 0.05f},
                                            *WorldWindowObj));
  OilText.setPosition(
      worldDeltaToScreen(
          {XCoord, 0.05f + WorldCharSize + 0.01f},
          *WorldWindowObj));
  StaminaText.setPosition(
      worldDeltaToScreen(
          {XCoord, 0.05f + (WorldCharSize + 0.01f) * 2},
          *WorldWindowObj));

  Window->draw(HealthText);
  Window->draw(OilText);
  Window->draw(StaminaText);
}

void Game::renderScene()
{
  Window->clear(sf::Color::Black);

  Backgr->draw(*RenderTex, *WorldWindowObj);

  for (auto& P : Passages)
    P->draw(*RenderTex, *WorldWindowObj);

  PlayerObj->draw(*RenderTex, *WorldWindowObj);

  for (auto& R : Rats)
  {
    R->draw(*RenderTex, *WorldWindowObj);
  }

  PlayerObj->getCurrTunnel()->draw(*RenderTex, *WorldWindowObj);

  PostprocessingShader->setUniform("world_window_center", WorldWindowObj->getCenter());
  PostprocessingShader->setUniform("world_window_dimensions", WorldWindowObj->getSize());
  PostprocessingShader->setUniform("world_light_pos", Lantern->getPosition());
  PostprocessingShader->setUniform("light_intensity", Lantern->getIntensity());
  PostprocessingShader->setUniform("backgr_intensity", BackgrIntensity);

  Window->draw(*ScreenSprite, PostprocessingShader.get());
  drawHUD();
  Window->display();
}

void Game::run()
{
  auto BeginTime = std::chrono::high_resolution_clock::now();
  long long FrameCount = 0;
  float FrameDrawingTimeMs = 0;

  while (Window->isOpen())
  {
    auto BeginFrameTime = std::chrono::high_resolution_clock::now();

    handleUserInput();

    processLogic(FrameDrawingTimeMs);

    renderScene();

    FrameCount++;

    auto FrameTimeDelta = std::chrono::high_resolution_clock::now() - BeginFrameTime;
    FrameDrawingTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(FrameTimeDelta).count();
  }

  auto TimeDelta = std::chrono::high_resolution_clock::now() - BeginTime;
  auto TimeDeltaMs = std::chrono::duration_cast<std::chrono::milliseconds>(TimeDelta);
  std::cout << "Avg FPS: " << 1000.0 / (static_cast<double>(TimeDeltaMs.count()) / FrameCount) << "\n";
  std::cout << "Total frames: " << FrameCount << "\n";
}
