#pragma once
#include "common.h"

class STransients {
public:
  STransients() {
    average = 0;
  }

  bool detect(float input) {
    average = (abs(input) + average * 200) / 201;
    if (abs(input) > average * 5) {
      return true;
    }
    else {
      return false;
    }
  }

  float process(float input) {
    if (detect(input)) {
      gain = mod;
    }
    else {
      gain = (gain * 500 + 1) / 501;
    }
    return input * gain;
  }


  inline void setMod(float m) {
    mod = mapGain(m);
  }

  inline void setLoudness(float loud) {
    loudness = loud;
  }

  //M range -1 ... 1
  float mapGain(float m) {
    if (m > 0) {
      return (1 + m * 3);
    }
    else {
      return m+1;
    }
  }

private:
  float average;
  float mod;
  float gain;
  float loudness;
};