//
// Created by anton on 4.4.24.
//

#ifndef ENGINE_SOUNDSYSTEM_HPP
#define ENGINE_SOUNDSYSTEM_HPP

#include <memory>
#include <string>

#include "types.hpp"

class SoundSystem {
public:
  SoundSystem();
  ~SoundSystem(); // Is needed for incomplete type SoundSystemImpl to compile (don't know why)

  SoundID loadSound(const std::string& FileName);
  SoundSourceID createSoundSource(const SoundSourceParams& Params);
  void playSoundSource(SoundSourceID SrcID);
  bool isPlaying(SoundSourceID SrcID);

private:
  std::unique_ptr<SoundSystemImpl> Impl;
};

#endif // ENGINE_SOUNDSYSTEM_HPP
