//
// Created by anton on 4.4.24.
//

#include "../facade/facade.hpp"

#include "SoundSystemImpl.hpp"

SoundSystem::SoundSystem() : Impl(new SoundSystemImpl) {}

SoundSystem::~SoundSystem() {}

SoundStructs::SoundID SoundSystem::loadSound(const std::string &FileName) {
  return Impl->loadSound(FileName);
}

SoundStructs::SoundSourceID SoundSystem::createSoundSource(
    const SoundStructs::SoundSourceParams& Params
    )
{
  return this->Impl->createSoundSource(Params);
}

void SoundSystem::playSoundSource(SoundStructs::SoundSourceID SrcID)
{
  this->Impl->playSoundSource(SrcID);
}

bool SoundSystem::isPlaying(SoundStructs::SoundSourceID SrcID)
{
  return this->Impl->isPlaying(SrcID);
}