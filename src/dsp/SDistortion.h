#pragma once
#include "common.h"

class SDistortion {
public:
  SDistortion() {
    drive = 0;
  }

  //Drive from 0 ... 1
  inline void setDrive(float d) {
    drive = d;
  }

  float soft(float input) {
    return tanh(input * (1 + drive * 10));
  }

  float hard(float input) {
    input = input * (1 + drive * 10);
    if (input > 1) {
      return 1;
    }
    else if (input < -1) {
      return -1;
    }
    else {
      return input;
    }
  }


private:
  float drive;
};