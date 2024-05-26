//
// Created by anton on 5/18/24.
//

#include "Animation.h"

Animation::Animation(const std::vector<std::string>& FramesFiles)
{
  for (const auto& FileName : FramesFiles)
  {
    auto Frame = std::make_unique<sf::Texture>();

    if (!Frame->loadFromFile(FileName))
      LOG_F(ERROR, "Cannot load file: %s", FileName.c_str());

    Frame->setSmooth(false);
    Frames.push_back(std::move(Frame));
  }

  AnimDuration = Frames.size() * FrameTime;
}

void Animation::update()
{
  if (!Playing)
    return;

  auto Duration = std::chrono::high_resolution_clock::now() - OldNow;
  auto DurationMs = std::chrono::duration_cast<std::chrono::milliseconds>(Duration);
  float PassedTimeSec = DurationMs.count() / 1000.0f;
  OldNow = std::chrono::high_resolution_clock::now();

  CurrAnimTime += PassedTimeSec;
  if (CurrAnimTime >= AnimDuration)
  {
    if (Loop)
      CurrAnimTime = 0;
    else
    {
      CurrAnimTime -= PassedTimeSec;
      stop();
    }
  }
}

void Animation::start()
{
  Playing = true;
  CurrAnimTime = 0;
  OldNow = std::chrono::high_resolution_clock::now();
}

void Animation::stop()
{
  Playing = false;
}

void Animation::setLoop(bool L)
{
  Loop = L;
}

void Animation::setFrameTime(float Time)
{
  FrameTime = Time;
  AnimDuration = FrameTime * Frames.size();
}

const Animation::FramesContainer& Animation::getFrames() const
{
  return Frames;
}

int Animation::getCurrFrameIndex() const
{
  return static_cast<int>(CurrAnimTime / FrameTime);
}
