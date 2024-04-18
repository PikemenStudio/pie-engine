//
// Created by anton on 4.4.24.
//

#include "../facade/SoundSystem.hpp"

#include "SoundSystemImpl.hpp"

SoundSystem::SoundSystem() : Impl(new SoundSystemImpl) {}

SoundSystem::~SoundSystem() {}

SoundID SoundSystem::loadSound(const std::string &FileName) {
  return Impl->loadSound(FileName);
}

SoundSourceID SoundSystem::createSoundSource(const SoundSourceParams& Params)
{
  return this->Impl->createSoundSource(Params);
}

void SoundSystem::playSoundSource(SoundSourceID SrcID)
{
  this->Impl->playSoundSource(SrcID);
}

bool SoundSystem::isPlaying(SoundSourceID SrcID)
{
  return this->Impl->isPlaying(SrcID);
}