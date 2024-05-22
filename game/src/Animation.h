//
// Created by anton on 5/18/24.
//

#ifndef ENGINE_ANIMATION_H
#define ENGINE_ANIMATION_H

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <loguru.hpp>

class Animation
{
public:
  using FramesContainer = std::vector<std::unique_ptr<sf::Texture>>;

  Animation(const std::vector<std::string>& FramesFiles)
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

  void update()
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

  void start()
  {
    Playing = true;
    CurrAnimTime = 0;
    OldNow = std::chrono::high_resolution_clock::now();
  }

  void stop()
  {
    Playing = false;
  }

  void setLoop(bool L)
  {
    Loop = L;
  }

  void setFrameTime(float Time)
  {
    FrameTime = Time;
    AnimDuration = FrameTime * Frames.size();
  }

  const FramesContainer& getFrames() const
  {
    return Frames;
  }

  int getCurrFrameIndex() const
  {
    return static_cast<int>(CurrAnimTime / FrameTime);
  }

private:
  std::vector<std::unique_ptr<sf::Texture>> Frames;
  float AnimDuration;
  float FrameTime = 0.1; // in seconds
  float CurrAnimTime = 0; // same
  bool Loop = true;
  bool Playing = false;

  std::chrono::high_resolution_clock::time_point OldNow;
};

#endif // ENGINE_ANIMATION_H
