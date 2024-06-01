//
// Created by anton on 5/13/24.
//

#include "LightSource.h"
#include "Background.h"

#include "utils.h"

#include <chrono>

using namespace std::chrono;

LightSource::LightSource(float X, float Y, float Intens)
    : WorldCoords(X, Y), Intensity(Intens), BaseIntensity(Intens)
{
  IntensityIntervalMs = 300.0f;
  IntervalStart = high_resolution_clock::now();
}

void LightSource::update()
{
  auto MsFromIntStart = duration_cast<milliseconds>((high_resolution_clock::now() - IntervalStart)).count();

  const float OilConsumptionSpeed = BaseIntensity < 0.45f ? 0.0033f : 0.02f;

  if (MsFromIntStart > IntensityIntervalMs)
  {
    Oil -= IntensityIntervalMs / 1000 * OilConsumptionSpeed;

    // generate new interval
    // change intensity
    Intensity = BaseIntensity + static_cast<float>(rand() % 1000) / 1000 * 0.03;
    IntensityIntervalMs = 50 + static_cast<float>(rand() % 1000) / 1000 * 250;
    IntervalStart = high_resolution_clock::now();
  }
}
