#pragma once
#include "cmath"
#include "common.h"
#define PI 3.14159265358979323846264

void getCoeffs(float(&coeffs)[5], float cutoff, float q, int type, float fgain) {
  float c = cutoff;
  float omega = PI * c;
  double sn = sin(omega);
  float cs = cos(omega);
  float alpha = sn / (2. * q);
  float gain_abs = pow(10, fgain / 40);
  float a0;
  switch (type) {

  case 0:
    //Lowpass
    coeffs[0] = (1 - cs) / 2;
    coeffs[1] = 1 - cs;
    coeffs[2] = (1 - cs) / 2;
    a0 = 1 + alpha;
    coeffs[3] = -2 * cs;
    coeffs[4] = 1 - alpha;
    break;

  case 1:
    //Highpass
    coeffs[0] = (1 + cs) / 2;
    coeffs[1] = -(1 + cs);
    coeffs[2] = (1 + cs) / 2;
    a0 = 1 + alpha;
    coeffs[3] = -2 * cs;
    coeffs[4] = 1 - alpha;
    break;
  case 2:
    //Bandpass
    coeffs[0] = alpha;
    coeffs[1] = 0;
    coeffs[2] = -alpha;
    a0 = 1 + alpha;
    coeffs[3] = -2 * cs;
    coeffs[4] = 1 - alpha;
    break;

  case 3:
    //Notch
    coeffs[0] = 1;
    coeffs[1] = -2 * cs;
    coeffs[2] = 1;
    a0 = 1 + alpha;
    coeffs[3] = -2 * cs;
    coeffs[4] = 1 - alpha;
    break;

  case 4:
    //Peak
    coeffs[0] = 1 + (alpha * gain_abs);
    coeffs[1] = -2 * cs;
    coeffs[2] = 1 - (alpha * gain_abs);
    a0 = 1 + (alpha / gain_abs);
    coeffs[3] = -2 * cs;
    coeffs[4] = 1 - (alpha / gain_abs);
    break;
  }

  coeffs[0] = coeffs[0] / a0;
  coeffs[1] = coeffs[1] / a0;
  coeffs[2] = coeffs[2] / a0;
  coeffs[3] = coeffs[3] / a0;
  coeffs[4] = coeffs[4] / a0;
}

class SBiquad {
public:
  SBiquad() {
    memset(coeffs, 0, sizeof(coeffs));
    memset(buf, 0, sizeof(buf));
  }

  //Sets the cutoff using a range of 0 ... 1
  void setLowpass(float cutoff) {
    getCoeffs(coeffs, cutoff, 0.707, 0, 0);
  }

  void setHighpass(float cutoff) {
    getCoeffs(coeffs, cutoff, 0.707, 1, 0);
  }

  float tick(float input) {
    float process = input * coeffs[0] + buf[0] * coeffs[1] + buf[1] * coeffs[2] - buf[2] * coeffs[3] - buf[3] * coeffs[4];
    buf[1] = buf[0];
    buf[0] = input;
    buf[3] = buf[2];
    buf[2] = process;
    return process;
  }

private:
  float coeffs[5];
  float buf[4];
};

class SFreqSplitter {
public:
  SFreqSplitter() {
    __m128 zero = _mm_set1_ps(0.f);
    b0 = b1 = b2 = a1 = a2 = buf0 = buf1 = buf2 = buf3 = zero;
  }

  void set(float lowpass, float highpass) {

    // ( lowpass left, highpass left, lowpass right, highpass right )
    float low[5];
    float high[5];
    getCoeffs(low, lowpass, 0.707, 0, 0);
    getCoeffs(high, highpass, 0.707, 1, 0);

    b0 = _mm_set_ps(high[0], low[0], high[0], low[0]);
    b1 = _mm_set_ps(high[1], low[1], high[1], low[1]);
    b2 = _mm_set_ps(high[2], low[2], high[2], low[2]);
    a1 = _mm_set_ps(high[3], low[3], high[3], low[3]);
    a2 = _mm_set_ps(high[4], low[4], high[4], low[4]);
  }

  // ( lowpass left, highpass left, lowpass right, highpass right )
  void tick(float* split, float left, float right) {
    __m128 input = _mm_set_ps(right, right, left, left);
    __m128 t1 = _mm_mul_ps(input, b0);
    __m128 t2 = _mm_mul_ps(buf0, b1);
    __m128 t3 = _mm_mul_ps(buf1, b2);
    __m128 t4 = _mm_mul_ps(buf2, a1);
    __m128 t5 = _mm_mul_ps(buf3, a2);

    __m128 process = _mm_add_ps(t1, t2);
    process = _mm_add_ps(process, t3);
    process = _mm_sub_ps(process, t4);
    process = _mm_sub_ps(process, t5);

    buf1 = buf0;
    buf0 = input;
    buf3 = buf2;
    buf2 = process;
    _mm_store_ps(split, process);
  }

private:
  __m128 b0;
  __m128 b1;
  __m128 b2;
  __m128 a1;
  __m128 a2;
  __m128 buf0;
  __m128 buf1;
  __m128 buf2;
  __m128 buf3;
};