//
// Created by anton on 6.4.24.
//

#ifndef ENGINE_SOUNDSYSTEMIMPL_HPP
#define ENGINE_SOUNDSYSTEMIMPL_HPP

#include "../facade/facade.hpp"

#include <al.h>
#include <alc.h>
#include <string>
#include <vector>

class SoundSystemImpl
{
public:
  SoundSystemImpl();
  ~SoundSystemImpl();

  SoundStructs::SoundID loadSound(const std::string& FileName);
  SoundStructs::SoundSourceID createSoundSource(const SoundStructs::SoundSourceParams& Params);
  void playSoundSource(SoundStructs::SoundSourceID SrcID);
  bool isPlaying(SoundStructs::SoundSourceID SrcID);

private:
  static bool IsInstanceAlreadyCreated;

  ALCdevice* OpenALDevice = nullptr;
  ALCcontext* OpenALContext = nullptr;
  std::vector<ALuint> Buffers;
  std::vector<ALuint> Sources;
};

#endif // ENGINE_SOUNDSYSTEMIMPL_HPP
