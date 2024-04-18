//
// Created by anton on 4.4.24.
//

#ifndef ENGINE_FACADE_HPP
#define ENGINE_FACADE_HPP

#include <memory>
#include <string>

#include "types.hpp"

struct SoundStructs
{
  using SoundID = unsigned int;
  using SoundSourceID = unsigned int;

  struct SoundSourceParams
  {
    float Pitch {};
    float Gain {};
    float PosX {}, PosY {}, PosZ {};
    float VelX {}, VelY {}, VelZ {};
    bool Looping = false;
    SoundID SndID;
  };
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Concept to get errors earlier if the Impl is not valid
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
template <typename T>
concept SoundModuleImpl = requires(T Obj) {
  // Must have method void test(int)
  {
    Obj.loadSound(std::declval<const std::string&>())
  } -> std::same_as<SoundStructs::SoundID>;
  {
    Obj.createSoundSource(std::declval<const SoundStructs::SoundSourceParams&>())
  } -> std::same_as<SoundStructs::SoundSourceID>;
  {
    Obj.playSoundSource(std::declval<SoundStructs::SoundSourceID>())
  } -> std::same_as<void>;
  {
    Obj.isPlaying(std::declval<SoundStructs::SoundSourceID>())
  } -> std::same_as<bool>;
};

class SoundSystemImpl;
class SoundSystem {
public:
  SoundSystem(const SoundSystem&) = delete;
  SoundSystem();
  ~SoundSystem(); // Is needed for incomplete type SoundSystemImpl to compile (don't know why)

  SoundStructs::SoundID loadSound(const std::string& FileName);
  SoundStructs::SoundSourceID createSoundSource(const SoundStructs::SoundSourceParams& Params);
  void playSoundSource(SoundStructs::SoundSourceID SrcID);
  bool isPlaying(SoundStructs::SoundSourceID SrcID);

private:
  std::unique_ptr<SoundSystemImpl> Impl;
};

#endif // ENGINE_FACADE_HPP
