#include <cmath>
#include "MapPot.hpp"

float mapPot(float value, float outMin, float outMax, float slope, bool invert, float inMin, float inMax) {
  // slope = 1 --> Linear; slope > 1 --> Exponentiell
  float normalizedValue;

  if (invert) {
    normalizedValue = 1-(value - inMin) / (inMax - inMin);
  }
  else {
    normalizedValue = (value - inMin) / (inMax - inMin);
  }
  if (slope != 1) {
    normalizedValue = pow(normalizedValue, slope);
  }
  float mappedValue = outMin + (normalizedValue * (outMax - outMin));
return mappedValue;
};