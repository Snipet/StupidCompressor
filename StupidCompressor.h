#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "IPlugPaths.h"
#include "src/dsp/SLFO.h"
#include "src/dsp/SCompressor.h"
#include "src/dsp/SFilter.h"

const int kNumPresets = 1;

enum EParams
{
  kGainIn = 0,
  kGainOut,
  kParamLFORateTempo,
  kLFODepth,
  kFoo,
  kLFOOffset,
  kThreshold,
  kRatio,
  kAttack,
  kRelease,
  kLowpassFreq,
  kHighpassFreq,
  kNumParams
};

enum EControlTags {
  kInfoText = 0,
  kLFOWindow,
  kCloseButton,
  kAboutButton,
  kCustomWindowSend,
  kAdvancedSkin,
  kCtrlTags
};

using namespace iplug;
using namespace igraphics;

class StupidCompressor final : public Plugin
{
public:
  StupidCompressor(const InstanceInfo& info);
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnIdle() override;
  //void OnParamChange(int idx) override;
  void OnParamChangeUI(int idx, EParamSource a) override;
  void OnParamChange(int idx) override;
  bool OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData) override;

private:
  IRECT b;
  SLFO lfo;
  std::vector<Point> points;
  ISender<3> mDisplaySender;
  float movingAverage;
  float movingAverageInput;
  SCompressor leftC;
  SCompressor rightC;
  float split[4];
  SFreqSplitter splitter;

  float left;
  float right;
  float bandLeft;
  float bandRight;
  float compressedLeft;
  float compressedRight;
  float outputLeft;
  float outputRight;
};


float clip(float n, float lower, float upper)
{
  n = (n > lower) * n + !(n > lower) * lower;
  return (n < upper) * n + !(n < upper) * upper;
}