//
// Created by anton on 5/19/24.
//

#ifndef ENGINE_DIMMINGTRANSITION_H
#define ENGINE_DIMMINGTRANSITION_H

#include <functional>

class DimmingTransition
{
public:
  DimmingTransition(float& Intens, const std::function<void()>& Func)
  : Intensity(Intens), TransitionFunc(Func)
  {
  }

  void play()
  {
    Playing = true;
    Intensity = 1;
  }

  void update()
  {
    if (!Playing)
      return;

    Intensity += Delta;

    if (Intensity < 0)
    {
      Intensity = 0;
//      TransitionFunc();
      Delta = -Delta;
    }
    else if (Intensity > 1)
    {
      Intensity = 1;
      Playing = false;
      Delta = -Delta;
    }
  }

private:
  float& Intensity;
  const std::function<void()>& TransitionFunc;
  bool Playing = false;
  float Delta = -0.05f;
};

#endif // ENGINE_DIMMINGTRANSITION_H
