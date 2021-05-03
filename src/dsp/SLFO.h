#pragma once
#include "common.h"
#include <vector>

#define kWaveformSize 2048
using namespace iplug;

class SLFO {
public:
  SLFO() {
    memset(waveform, 0, sizeof(waveform));
    samplesPerBeat = 0;
  }

  std::vector<Point> getPoints() {
    return points;
  }

  void setPoints(std::vector<Point> input) {
    points = input;
    //points.resize(0);
    Point p;
    p.x = 0;
    p.y = 0;
    p.uid = 1;
    points.push_back(p);

    //Point p2;
    //p2.x = 0.5;
    //p2.y = 1;
    //p2.uid = 2;
    //points.push_back(p2);

    Point p1;
    p1.x = 1;
    p1.y = 0;
    p1.uid = 3;
    points.push_back(p1);
    std::sort(points.begin(), points.end()); //Don't forget this, kind of
    makeWaveform();
  }

  void makeWaveform() {
    float p = 0;
    for (int i = 0; i < kWaveformSize; i++) {
      waveform[i] = getValueAt(p);
      p += 1.f / (float)kWaveformSize;
    }
  }


  //Gets the value determined by the points at a given phase from 0...1
  float getValueAt(float phase) {
    assert(phase >= 0 && phase <= 1);
    bool foundFoo = true;
    int index = -1;
    for (int i = 0; i < points.size() - 1; i++) {
      if (phase >= points[i].x && phase <= points[i+1].x && foundFoo) {
        index = i;
        foundFoo = false;
      }
    }
    assert(index != -1);
    float range = points[index + 1].x - points[index].x;
    float mu = (phase - points[index].x) / range;
    return LinearInterpolate(points[index].y, points[index + 1].y, mu);
  }

  float waveformLookup(float p) {
    p = p * kWaveformSize;
    int bottom = floor(p);
    return (waveform[bottom] + waveform[bottom + 1]) / 2;
  }

  void setPhaseIncrement(float sampleRate, float tempo) {
    samplesPerBeat = sampleRate * (60.0 / (tempo == 0.0 ? 1.0 : tempo));
  }

  void setScalar(float scalar) {
    mQNScalar = scalar;
  }

  float ProcessBlock(int nFrames, float qnPos, bool transportIsRunning, float index) {
    float t = 1.f / mQNScalar;
    double sampleAccurateQnPos = qnPos + ((double)nFrames / samplesPerBeat);
    if (transportIsRunning) {
      phase = fmod(qnPos, t) / t;
    }
    else {
      phase = fmod(phase + 1.f / samplesPerBeat * mQNScalar * nFrames, 1.f);
    }

    //phase = fmod(phase + 0.01, 1.f);
    return getInterpolated(phase*(kWaveformSize-1));
  }


  //Takes in a value from 0 ... kWaveformSize
  float getInterpolated(float p) {
    int bottom = floor(p);
    return (waveform[bottom] + waveform[bottom + 1]) / 2;
  }

private:
  std::vector<Point> points;
  float waveform[kWaveformSize];
  float tempoDivisor;
  float phaseIncrement;
  float phase;
  float samplesPerBeat;
  float mQNScalar;

  float LinearInterpolate(
    float y1, float y2,
    float mu)
  {
    return(y1 * (1 - mu) + y2 * mu);
  }

};