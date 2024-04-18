//
// Created by anton on 6.4.24.
//

#ifndef ENGINE_TYPES_HPP
#define ENGINE_TYPES_HPP

typedef unsigned int SoundID;
typedef unsigned int SoundSourceID;

struct SoundSourceParams
{
  float Pitch {};
  float Gain {};
  float PosX {}, PosY {}, PosZ {};
  float VelX {}, VelY {}, VelZ {};
  bool Looping = false;
  SoundID SndID;
};

class SoundSystemImpl;

#endif // ENGINE_TYPES_HPP
