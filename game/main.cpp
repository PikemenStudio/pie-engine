//
// Created by anton on 5/12/24.
//

#include <iostream>
#include <vector>

#include <noise/noise.h>

#include <sound/facade/facade.hpp>

class Floor
{
public:
  Floor(double StartX, double StepX, int PointsCount, const noise::module::Perlin& NoiseGen)
  {
    this->StartX = StartX;
    this->StepX = StepX;
    double CurrX = StartX;

    while (PointsCount--)
    {
      WorldCoordsY.push_back(NoiseGen.GetValue(CurrX, 0, 0));
      CurrX += StepX;
    }
  }

  double getStartX() const
  {
    return StartX;
  }

  double getStepX() const
  {
    return StepX;
  }

  const std::vector<double>& getCoords() const
  {
    return WorldCoordsY;
  }

private:
  double StartX = 0.0;
  double StepX = 0.1;
  std::vector<double> WorldCoordsY;
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
  noise::module::Perlin NoiseGen;
  NoiseGen.SetSeed(time(nullptr));
  Floor FloorObj(0, 0.1, 10, NoiseGen);

  const std::vector<double>& FloorCoords = FloorObj.getCoords();

  for (auto Coord : FloorCoords)
  {
    std::cout << Coord << " ";
  }
  std::cout << "\n";

  return 0;
}