//
// Created by anton on 5/12/24.
//

#include <iostream>
#include <vector>

#include <noise/noise.h>

#include <sound/facade/facade.hpp>

#include <SFML/Graphics.hpp>

sf::Vector2f worldCoordsToScreen(const sf::Vector2f& WorldCoords)
{
  return sf::Vector2f((WorldCoords.x + 1) * 800 / 2, (1 - WorldCoords.y) * 600 / 2);
}

class Floor
{
public:
  Floor(float StartX, float StepX, int PointsCount)
  {
    noise::module::Perlin NoiseGen;
    NoiseGen.SetSeed(time(nullptr));

    this->StartX = StartX;
    this->StepX = StepX;
    float CurrX = StartX;

    while (PointsCount--)
    {
      WorldCoordsY.push_back((NoiseGen.GetValue(CurrX, 0, 0) - 1) / 2);
      CurrX += StepX;
    }
  }

  Floor(float StartX, float EndX, float StepX)
  : Floor(StartX, StepX, static_cast<int>((EndX - StartX) / StepX) + 1)
  {
  }

  void draw(sf::RenderWindow& Win) const
  {
    auto Circle = sf::CircleShape(5);
    Circle.setFillColor(sf::Color::Black);

    for (int I = 0; I < WorldCoordsY.size() - 1; I++)
    {
      float WorldX1 = StartX + I * StepX;
      float WorldY1 = WorldCoordsY[I];
      float WorldX2 = StartX + (I + 1) * StepX;
      float WorldY2 = WorldCoordsY[I + 1];

      sf::Vector2f ScreenXY1 = worldCoordsToScreen(sf::Vector2f(WorldX1, WorldY1));
      sf::Vector2f ScreenXY2 = worldCoordsToScreen(sf::Vector2f(WorldX2, WorldY2));

      sf::ConvexShape FloorPart;
      // resize it to 5 points
      FloorPart.setPointCount(4);

      // define the points
      FloorPart.setPoint(0, ScreenXY1);
      FloorPart.setPoint(1, ScreenXY2);
      FloorPart.setPoint(2, sf::Vector2f(ScreenXY2.x, 600));
      FloorPart.setPoint(3, sf::Vector2f(ScreenXY1.x, 600));

      FloorPart.setFillColor(sf::Color::Black);

      Win.draw(FloorPart);
    }
  }

  float getStartX() const
  {
    return StartX;
  }

  float getStepX() const
  {
    return StepX;
  }

  const std::vector<float>& getCoords() const
  {
    return WorldCoordsY;
  }

private:
  float StartX = 0.0;
  float StepX = 0.1;
  std::vector<float> WorldCoordsY;
};

void soundTest()
{
  SoundFacade Snd;
  auto SndId = Snd.ImplInstance.loadSound("../../game/resources/sound_test_mono.wav");
  auto SndSrcParams = SoundStructs::SoundSourceParams {
      .Pitch = 1.0f, .Gain = 1.0f,
      .PosX = 0, .PosY = 0, .PosZ = 0,
      .VelX = 0, .VelY = 0, .VelZ = 0,
      .Looping = false,
      .SndID = SndId
  };
  auto SndSrc = Snd.ImplInstance.createSoundSource(SndSrcParams);
  Snd.ImplInstance.playSoundSource(SndSrc);

  while (Snd.ImplInstance.isPlaying(SndSrc))
  {
  }
}

int main()
{
//  Floor FloorObj(-1, 0.01, 201);
  Floor FloorObj(-1, 1, 0.01f);

  // create the window
  sf::RenderWindow Window(sf::VideoMode(800, 600), "Game");

  // run the program as long as the window is open
  while (Window.isOpen())
  {
    // check all the window's events that were triggered since the last iteration of the loop
    sf::Event Event;
    while (Window.pollEvent(Event))
    {
      // "close requested" event: we close the window
      if (Event.type == sf::Event::Closed)
        Window.close();
    }

    // clear the window with white color
    Window.clear(sf::Color::White);

    FloorObj.draw(Window);

    // end the current frame
    Window.display();
  }

  return 0;
}