#pragma once
#include "common.h"
#include "cmath"
//Lightweight limiter, light-ish on CPU and not a lot of parameters
class SLightLimiter {
public:
  SLightLimiter() {
    sampleRate = 44100;
    release = 50000;
    gain = 0;
    threshold = 0.5;
    lastOutput = 0;
    drive = 1;
    sustainTick = 0;
    sustain = 500000;
    peak = threshold;
  }

  inline void setSampleRate(float fs) {
    sampleRate = fs;
  }

  inline void setRelease(float r) {
    release = r;
  }

  inline void setDrive(float d) {
    drive = (1 + d * 10);
  }

  float tick(float input) {
    input = input * drive;
    if ((input > threshold) || (input < -threshold)) {
      if (abs(gain * input) > threshold) {
        gain = threshold / abs(input);
      }

    }
    else {
        gain = (gain * release + threshold) / (release + 1);
    }

    peak = (peak * 100000 + threshold) / 100001;

    lastOutput = input * gain;
    return lastOutput;
  }

private:
  float sampleRate;
  float release;
  float gain;
  float threshold;
  float lastOutput;
  float drive;
  float sustain;
  float sustainTick;
  float peak;
};