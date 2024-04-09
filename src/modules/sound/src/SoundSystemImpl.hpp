//
// Created by anton on 6.4.24.
//

#ifndef ENGINE_SOUNDSYSTEMIMPL_HPP
#define ENGINE_SOUNDSYSTEMIMPL_HPP

#include "../facade/types.hpp"

#include <al.h>
#include <alc.h>
#include <string>
#include <vector>

class SoundSystemImpl
{
public:
  SoundSystemImpl();
  ~SoundSystemImpl();

  SoundID loadSound(const std::string& FileName);
  SoundSourceID createSoundSource(const SoundSourceParams& Params);
  void playSoundSource(SoundSourceID SrcID);
  bool isPlaying(SoundSourceID SrcID);

private:
  static bool IsInstanceAlreadyCreated;

  ALCdevice* OpenALDevice = nullptr;
  ALCcontext* OpenALContext = nullptr;
  std::vector<ALuint> Buffers;
  std::vector<ALuint> Sources;
};

#endif // ENGINE_SOUNDSYSTEMIMPL_HPP
