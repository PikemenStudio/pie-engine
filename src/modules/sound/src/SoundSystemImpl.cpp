//
// Created by anton on 6.4.24.
//

#include "SoundSystemImpl.hpp"

#include <array>
#include <loguru.hpp>
#include <sndfile.h>
#include <stdexcept>

bool SoundSystemImpl::IsInstanceAlreadyCreated = false;

SoundSystemImpl::SoundSystemImpl() {
  LOG_F(INFO, "Creating sound system");

  if (!IsInstanceAlreadyCreated)
    IsInstanceAlreadyCreated = true;
  else
    throw std::runtime_error("Cannot have another sound system");

  LOG_F(INFO, "Opening an OpenAL device");
  OpenALDevice = alcOpenDevice(nullptr);
  if (!OpenALDevice)
    throw std::runtime_error("Failed to open an OpenAL device");

  LOG_F(INFO, "Creating an OpenAL context");
  OpenALContext = alcCreateContext(OpenALDevice, nullptr);
  if (!OpenALContext)
    throw std::runtime_error("Could not create audio context");

  if (!alcMakeContextCurrent(OpenALContext))
    throw std::runtime_error("Could not make audio context current");
}

SoundSystemImpl::~SoundSystemImpl() {
  LOG_F(INFO, "Destroying sound system");

  alDeleteSources(this->Sources.size(), this->Sources.data());
  alDeleteBuffers(this->Buffers.size(), this->Buffers.data());

  alcMakeContextCurrent(nullptr);
  alcDestroyContext(OpenALContext);
  alcCloseDevice(OpenALDevice);
}

SoundStructs::SoundID SoundSystemImpl::loadSound(const std::string &FileName) {
  LOG_F(INFO, "Loading file: %s", FileName.c_str());

  SF_INFO Info;
  SNDFILE* File = sf_open(FileName.c_str(), SFM_READ, &Info);
  if (!File)
  {
    throw std::runtime_error("Failed to open file");
  }

  std::vector<uint16_t> Data;
  std::array<int16_t, 4096> ReadBuf;
  size_t ReadSize;
  while ((ReadSize = sf_read_short(File, ReadBuf.data(), ReadBuf.size())) != 0) {
    Data.insert(Data.end(), ReadBuf.begin(), ReadBuf.begin() + ReadSize);
  }

  sf_close(File);

  ALuint Buffer;
  alGenBuffers(1, &Buffer);
  this->Buffers.push_back(Buffer);

  ALenum Format = Info.channels == 1? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
  if (Format == AL_FORMAT_STEREO16 || Format == AL_FORMAT_STEREO8)
  {
    LOG_F(INFO, "Cannot put sound in space: stereo format");
  }

  alBufferData(Buffer, Format,Data.data(), Data.size() * sizeof(uint16_t), Info.samplerate);

  return Buffer;
}

SoundStructs::SoundSourceID SoundSystemImpl::createSoundSource(
    const SoundStructs::SoundSourceParams& Params
) {
  LOG_F(INFO, "Creating sound source");

  ALuint Source;
  alGenSources(1, &Source);
  this->Sources.push_back(Source);

  alSourcef(Source, AL_PITCH, Params.Pitch);
  alSourcef(Source, AL_GAIN, Params.Gain);
  alSource3f(Source, AL_POSITION, Params.PosX, Params.PosY, Params.PosZ);
  alSource3f(Source, AL_VELOCITY, Params.VelX, Params.VelY, Params.VelZ);
  alSourcei(Source, AL_LOOPING, Params.Looping);
  alSourcei(Source, AL_BUFFER, Params.SndID);

  return Source;
}

void SoundSystemImpl::playSoundSource(SoundStructs::SoundSourceID SrcID)
{
  LOG_F(INFO, "Start playing sound source");
  alSourcePlay(SrcID);
}

bool SoundSystemImpl::isPlaying(SoundStructs::SoundSourceID SrcID) {
  ALint State = AL_NONE;
  alGetSourcei(SrcID, AL_SOURCE_STATE, &State);
  return State == AL_PLAYING;
}