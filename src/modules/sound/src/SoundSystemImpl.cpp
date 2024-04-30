//
// Created by anton on 6.4.24.
//

#include "../facade/facade.hpp"

#include <al.h>
#include <alc.h>
#include <array>
#include <loguru.hpp>
#include <sndfile.h>
#include <stdexcept>
#include <string>
#include <vector>

namespace sound_impls {

class SoundALData {
public:
  ALCdevice *OpenALDevice = nullptr;
  ALCcontext *OpenALContext = nullptr;
  std::vector<ALuint> Buffers;
  std::vector<ALuint> Sources;
};

// bool SoundALImpl::IsInstanceAlreadyCreated = false;

SoundALImpl::SoundALImpl() : Data(new sound_impls::SoundALData) {
  LOG_F(INFO, "Creating sound system");

  //  if (!IsInstanceAlreadyCreated)
  //    IsInstanceAlreadyCreated = true;
  //  else
  //    throw std::runtime_error("Cannot have another sound system");

  LOG_F(INFO, "Opening an OpenAL device");
  Data->OpenALDevice = alcOpenDevice(nullptr);
  if (!Data->OpenALDevice)
    throw std::runtime_error("Failed to open an OpenAL device");

  LOG_F(INFO, "Creating an OpenAL context");
  Data->OpenALContext = alcCreateContext(Data->OpenALDevice, nullptr);
  if (!Data->OpenALContext)
    throw std::runtime_error("Could not create audio context");

  if (!alcMakeContextCurrent(Data->OpenALContext))
    throw std::runtime_error("Could not make audio context current");
}

SoundALImpl::~SoundALImpl() {
  LOG_F(INFO, "Destroying sound system");

  alDeleteSources(this->Data->Sources.size(), this->Data->Sources.data());
  alDeleteBuffers(this->Data->Buffers.size(), this->Data->Buffers.data());

  alcMakeContextCurrent(nullptr);
  alcDestroyContext(Data->OpenALContext);
  alcCloseDevice(Data->OpenALDevice);
}

SoundStructs::SoundID SoundALImpl::loadSound(const std::string &FileName) {
  LOG_F(INFO, "Loading file: %s", FileName.c_str());

  SF_INFO Info;
  SNDFILE *File = sf_open(FileName.c_str(), SFM_READ, &Info);
  if (!File) {
    throw std::runtime_error("Failed to open file");
  }

  std::vector<uint16_t> SoundData;
  std::array<int16_t, 4096> ReadBuf;
  size_t ReadSize;
  while ((ReadSize = sf_read_short(File, ReadBuf.data(), ReadBuf.size())) !=
         0) {
    SoundData.insert(SoundData.end(), ReadBuf.begin(),
                     ReadBuf.begin() + ReadSize);
  }

  sf_close(File);

  ALuint Buffer;
  alGenBuffers(1, &Buffer);
  this->Data->Buffers.push_back(Buffer);

  ALenum Format = Info.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  if (Format == AL_FORMAT_STEREO16 || Format == AL_FORMAT_STEREO8) {
    LOG_F(INFO, "Cannot put sound in space: stereo format");
  }

  alBufferData(Buffer, Format, SoundData.data(),
               SoundData.size() * sizeof(uint16_t), Info.samplerate);

  return Buffer;
}

SoundStructs::SoundSourceID
SoundALImpl::createSoundSource(const SoundStructs::SoundSourceParams &Params) {
  LOG_F(INFO, "Creating sound source");

  ALuint Source;
  alGenSources(1, &Source);
  this->Data->Sources.push_back(Source);

  alSourcef(Source, AL_PITCH, Params.Pitch);
  alSourcef(Source, AL_GAIN, Params.Gain);
  alSource3f(Source, AL_POSITION, Params.PosX, Params.PosY, Params.PosZ);
  alSource3f(Source, AL_VELOCITY, Params.VelX, Params.VelY, Params.VelZ);
  alSourcei(Source, AL_LOOPING, Params.Looping);
  alSourcei(Source, AL_BUFFER, Params.SndID);

  return Source;
}

void SoundALImpl::playSoundSource(SoundStructs::SoundSourceID SrcID) {
  LOG_F(INFO, "Start playing sound source");
  alSourcePlay(SrcID);
}

bool SoundALImpl::isPlaying(SoundStructs::SoundSourceID SrcID) {
  ALint State = AL_NONE;
  alGetSourcei(SrcID, AL_SOURCE_STATE, &State);
  return State == AL_PLAYING;
}

} // namespace sound_impls