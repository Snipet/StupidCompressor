#pragma once
#include "common.h"

class SCompressor {
public:
  SCompressor() {
    movingAverage = 0;
    attack = 0;
    release = 0;
    ratio = 1;
    threshold = 1;
    gain = 1;
    lastValue = 0;
    coeff = 0;
  }

  inline void setThreshold(float t) {
    threshold = t;
    setSubstractor();
  }

  inline void setAttack(float a) {
    attack = a * (sampleRate / 1000);
  }

  inline void setRelease(float r) {
    release = r * (sampleRate / 1000);
  }

  inline void setRatio(float r) {
    ratio = r;
    setSubstractor();
  }

  inline void setSampleRate(float fs) {
    sampleRate = fs;
  }

  float returnGain() {
    return gain;
  }

  float tick(float input) {
    movingAverage = (movingAverage * 50 + abs(input)) / 51;
    if (movingAverage*2 > threshold) {
      gain = (gain * attack + getGain(movingAverage*2)) / (attack + 1);
    }
    else {
      gain = (gain * release+1) / (release + 1);
    }
    lastValue = input * gain;
    return lastValue;
  }


private:
  float movingAverage;
  float attack;
  float release;
  float ratio;
  float threshold;
  float gain;
  float sampleRate;
  float subtractor;
  float reverseRatio;
  float lastValue;
  float coeff;

  void setSubstractor() {
    subtractor = ((1 / ratio) / (1 / threshold)) - threshold;
    reverseRatio = 1 / ratio;
  }


  float getGain(float x) {
    //  if (x < threshold) {
    //    return x;
    //  }
    //  else {
    //    return (reverseRatio * movingAverage) - (subtractor);
    //  }
    return ((reverseRatio * x) - (subtractor)) / x;
  }

};