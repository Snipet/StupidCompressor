#include "StupidCompressor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "common.h"
#include "src/interface/SKnob.h"
#include "src/interface/SSkin.h"
#include "src/interface/SSidebar.h"
#include "src/interface/SLFOWindow.h"
#include "src/interface/SInfoText.h"
#include "src/interface/SCloseButton.h"
#include "src/interface/SOpenLFOButton.h"
#include "src/interface/SAboutWindow.h"
#include "src/interface/STextLogoButton.h"
#include "src/interface/SCustomInterface.h"
#include "src/interface/SRatioViewer.h"
#include "src/interface/STextButton.h"

StupidCompressor::StupidCompressor(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kGainIn)->InitDouble("In Gain", 100., 0., 500., 0.01, "%");
  GetParam(kGainOut)->InitDouble("Out Gain", 100., 0., 500., 0.01, "%");
  GetParam(kThreshold)->InitDouble("Threshold", 1., 0.001, 1, 0.001, "");
  GetParam(kAttack)->InitDouble("Attack", 50., 0.1, 1000, 0.01, "ms", 0, "", IParam::ShapePowCurve(3));
  GetParam(kRelease)->InitDouble("Release", 50., 5., 1000, 0.01, "ms", 0, "", IParam::ShapePowCurve(3));
  GetParam(kRatio)->InitDouble("Ratio", 2, 1, 100, 0.1, "", 0, "", IParam::ShapePowCurve(3));


#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {

    const IVStyle style{
   true, // Show label
   true, // Show value
   {
     DEFAULT_BGCOLOR, // Background
     DEFAULT_FGCOLOR, // Foreground
     DEFAULT_PRCOLOR, // Pressed
     IColor(50,0,0,0), // Frame
     DEFAULT_HLCOLOR, // Highlight
     DEFAULT_SHCOLOR, // Shadow
     COLOR_BLACK, // Extra 1
     DEFAULT_X2COLOR, // Extra 2
     DEFAULT_X3COLOR  // Extra 3
   }, // Colors
   IText(12.f, EAlign::Center) // Label text
    };

    auto closeAdvanced = [pGraphics, this](IControl* pCaller) mutable {
      pGraphics->RemoveControlWithTag(kAdvancedSkin);
      pGraphics->RemoveControlWithTag(kCloseButton);
    };

    auto openAdvanced = [pGraphics, this, closeAdvanced](IControl* pCaller) mutable {
      pGraphics->AttachControl(new SSkin(IRECT(0, 0, PLUG_WIDTH, PLUG_HEIGHT), 1), kAdvancedSkin);
      pGraphics->AttachControl(new SCloseButton(IRECT(865, 5, 895, 35), closeAdvanced), kCloseButton);
    };

    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    b = pGraphics->GetBounds();

    pGraphics->AttachControl(new SSkin(IRECT(0, 0, PLUG_WIDTH, PLUG_HEIGHT), 0));
    SKnob_Style style1 = SKnob_Style(IColor(255,50,50,50), IColor(255,200,200,200), IColor(255,240, 240, 240), 4.f, 4.f, IColor(255,40,150,255), false);
    SKnob_Style style2 = SKnob_Style(IColor(255, 217, 217, 217), IColor(255, 38, 38, 38), IColor(255, 38, 38, 38), 4.f, 4.f, IColor(255,40,150,255), true);
    pGraphics->AttachControl(new SKnob(b.GetCentredInside(100).GetHShifted(-400).GetVShifted(-85), kThreshold, style1));
    pGraphics->AttachControl(new SKnob(b.GetCentredInside(100).GetHShifted(-300).GetVShifted(-85), kRatio, style1));
    pGraphics->AttachControl(new SKnob(b.GetCentredInside(100).GetHShifted(-400).GetVShifted(15), kAttack, style1));
    pGraphics->AttachControl(new SKnob(b.GetCentredInside(100).GetHShifted(-300).GetVShifted(15), kRelease, style1));

    pGraphics->AttachControl(new SKnob(b.GetCentredInside(100).GetHShifted(-400).GetVShifted(115), kGainIn, style1));
    pGraphics->AttachControl(new SKnob(b.GetCentredInside(100).GetHShifted(-300).GetVShifted(115), kGainOut, style1));

    pGraphics->AttachControl(new STextButton(IRECT(800,5,900,35), openAdvanced, "Advanced"));

    pGraphics->AttachControl(new SCustomInterface<3, 512>(IRECT(460, 60, 880, 330), kThreshold), kCustomWindowSend);
    pGraphics->AttachControl(new SRatioViewer<1, 512>(IRECT(200, 60, 440, 330), {kThreshold, kRatio}));
    //pGraphics->AttachControl(new SInfoText(IRECT(2,335,500,350), kGainOut), kInfoText);
    pGraphics->AttachControl(new STextLogoButton(IRECT(5,0,190,40)));


  };
  points.resize(0);
  Point p;
  p.x = 0.5;
  p.y = 1;
  p.uid = 1;
  points.push_back(p);
  lfo.setPoints(points);
  movingAverage = 0;
  leftC.setSampleRate(GetSampleRate());
  rightC.setSampleRate(GetSampleRate());
#endif
}

#if IPLUG_DSP
void StupidCompressor::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gainIn = GetParam(kGainIn)->Value() / 100.;
  const double gainOut = GetParam(kGainOut)->Value() / 100.;
  for (int s = 0; s < nFrames; s++) {
    outputs[0][s] = leftC.tick(gainIn * inputs[0][s]) * gainOut;
    outputs[1][s] = rightC.tick(gainIn * inputs[1][s]) * gainOut;
    movingAverage = (movingAverage * 3000 + abs(outputs[0][s]) + abs(outputs[1][s])) / 3002;
    movingAverageInput = (movingAverageInput * 3000 + abs(inputs[0][s] * gainIn) + abs(inputs[1][s] * gainIn)) / 3002;
    }
  
  mDisplaySender.PushData({ kCustomWindowSend, {movingAverage*1.75f, leftC.returnGain(), movingAverageInput*1.75f} });

}


void StupidCompressor::OnIdle() {
  mDisplaySender.TransmitData(*this);
}

void StupidCompressor::OnParamChangeUI(int idx, EParamSource a) {
  if (auto graphics = GetUI()) {
    GetUI()->RemoveControlWithTag(kInfoText);
    GetUI()->AttachControl(new SInfoText(IRECT(500,335,900,350), idx), kInfoText);
  }
}

#endif

bool StupidCompressor::OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData) {

  auto pConverted = reinterpret_cast<const std::vector<Point>*>(pData);
  points = *pConverted;
  switch (msgTag) {
  case kSendPoints:
    lfo.setPoints(*pConverted);
  }
  return true;
}

void StupidCompressor::OnParamChange(int idx) {
  auto value = GetParam(idx)->Value();
  switch (idx) {
  case kThreshold:
    leftC.setThreshold(value);
    rightC.setThreshold(value);
    break;

  case kRatio:
    leftC.setRatio(value);
    rightC.setRatio(value);
    break;

  case kAttack:
    leftC.setAttack(value);
    rightC.setAttack(value);
    break;

  case kRelease:
    leftC.setRelease(value);
    rightC.setRelease(value);
    break;
  }
}