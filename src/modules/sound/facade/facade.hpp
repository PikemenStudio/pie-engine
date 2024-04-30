//
// Created by anton on 4.4.24.
//

#ifndef ENGINE_FACADE_HPP
#define ENGINE_FACADE_HPP

#include <memory>
#include <string>

#include "types.hpp"

struct SoundStructs {
  using SoundID = unsigned int;
  using SoundSourceID = unsigned int;

  struct SoundSourceParams {
    float Pitch{};
    float Gain{};
    float PosX{}, PosY{}, PosZ{};
    float VelX{}, VelY{}, VelZ{};
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
    Obj.loadSound(std::declval<const std::string &>())
  } -> std::same_as<SoundStructs::SoundID>;
  {
    Obj.createSoundSource(
        std::declval<const SoundStructs::SoundSourceParams &>())
  } -> std::same_as<SoundStructs::SoundSourceID>;
  {
    Obj.playSoundSource(std::declval<SoundStructs::SoundSourceID>())
  } -> std::same_as<void>;
  {
    Obj.isPlaying(std::declval<SoundStructs::SoundSourceID>())
  } -> std::same_as<bool>;
};

#define SOUND_IMPL(name)                                                       \
  class Sound##name##Data;                                                     \
  class Sound##name##Impl {                                                    \
  public:                                                                      \
    Sound##name##Impl(const Sound##name##Impl &) = delete;                     \
    Sound##name##Impl();                                                       \
    ~Sound##name##Impl();                                                      \
                                                                               \
    SoundStructs::SoundID loadSound(const std::string &FileName);              \
    SoundStructs::SoundSourceID                                                \
    createSoundSource(const SoundStructs::SoundSourceParams &Params);          \
    void playSoundSource(SoundStructs::SoundSourceID SrcID);                   \
    bool isPlaying(SoundStructs::SoundSourceID SrcID);                         \
                                                                               \
  private:                                                                     \
    std::unique_ptr<Sound##name##Data> Data;                                   \
  };

namespace sound_impls {
SOUND_IMPL(AL)
} // namespace sound_impls

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Facade for Sound
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class SoundFacade {
public:
  using Impl = sound_impls::SoundALImpl;

  SoundFacade() // Maybe need to pass Props here
  {}

public:
  [[maybe_unused]] Impl ImplInstance;
};

#endif // ENGINE_FACADE_HPP
