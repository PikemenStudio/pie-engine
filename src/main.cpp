#include <iostream>
//
// Created by FullHat on 28/03/2024.
//
#include <loguru.hpp>

#include "modules/sound/facade/SoundSystem.hpp"

int main(int argc, char *argv[]) {
  loguru::init(argc, argv);
  loguru::add_file("everything.log", loguru::Append, loguru::Verbosity_MAX);

  SoundSystem SndSystem;
  SoundID AvantGarde = SndSystem.loadSound("../../temp-resources/untitled_mono.wav");
  SoundSourceID Src1 = SndSystem.createSoundSource(SoundSourceParams {
      .Pitch = 1.0, .Gain = 1.0,
      .PosX = 0.2, .PosY = 0, .PosZ = -0.5,
      .VelX = 0, .VelY = 0, .VelZ = 0,
      .Looping = false,
      .SndID = AvantGarde
  });
  SoundSourceID Src2 = SndSystem.createSoundSource(SoundSourceParams {
      .Pitch = 0.5, .Gain = 1.2,
      .PosX = -0.3, .PosY = 0, .PosZ = -0.5,
      .VelX = 0, .VelY = 0, .VelZ = 0,
      .Looping = false,
      .SndID = AvantGarde
  });
  SoundSourceID Src3 = SndSystem.createSoundSource(SoundSourceParams {
      .Pitch = 1.5, .Gain = 1.2,
      .PosX = -0.3, .PosY = 0, .PosZ = 0.5,
      .VelX = 0, .VelY = 0, .VelZ = 0,
      .Looping = false,
      .SndID = AvantGarde
  });

  SndSystem.playSoundSource(Src1);
  SndSystem.playSoundSource(Src2);
  SndSystem.playSoundSource(Src3);

  while (SndSystem.isPlaying(Src1) || SndSystem.isPlaying(Src2) || SndSystem.isPlaying(Src3))
  {
    usleep(10000);
  }

  return 0;
}