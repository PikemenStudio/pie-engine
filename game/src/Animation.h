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

  Animation(const std::vector<std::string>& FramesFiles);

  void update();
  void start();
  void stop();

  void setLoop(bool L);
  void setFrameTime(float Time);

  const FramesContainer& getFrames() const;
  int getCurrFrameIndex() const;

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
