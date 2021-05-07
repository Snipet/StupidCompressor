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
#include "src/interface/SToggleButton.h"
#include "src/interface/SOptionChooser.h"

StupidCompressor::StupidCompressor(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kGainIn)->InitDouble("In Gain", 100., 0., 1000., 0.01, "%");
  GetParam(kGainOut)->InitDouble("Out Gain", 100., 0.,1000., 0.01, "%");
  GetParam(kThreshold)->InitDouble("Compression", 0., 0.001, 100, 0.001, "");
  GetParam(kAttack)->InitDouble("Attack", 50., 0, 1000, 0.01, "ms", 0, "", IParam::ShapePowCurve(3));
  GetParam(kRelease)->InitDouble("Release", 50., 5., 1000, 0.01, "ms", 0, "", IParam::ShapePowCurve(3));
  GetParam(kRatio)->InitDouble("Ratio", 2, 1, 100, 0.1, "", 0, "", IParam::ShapePowCurve(3));
  GetParam(kLowpassFreq)->InitDouble("Lowpass Cutoff", 15000, 10, 22000, 0.1, "hz", 0, "", IParam::ShapePowCurve(3));
  GetParam(kHighpassFreq)->InitDouble("Highpass Cutoff", 500, 10, 22000, 0.1, "hz", 0, "", IParam::ShapePowCurve(3));
  GetParam(kBandpassCompress)->InitBool("Bandpass Compression", false);
  GetParam(kClipPower)->InitBool("Clipping", false);
  GetParam(kClipType)->InitEnum("Cliping Type", 2, {"Soft", "Hard", "Limit" });
  GetParam(kCompressMode)->InitEnum("Compression Type", 0, { "Feed Forward", "Feedback"});
  GetParam(kTransientPower)->InitBool("Transients", false);
  GetParam(kTransients)->InitDouble("Transients", 0, -100.f, 100.f, 0.01, "%");
  GetParam(kDrive)->InitDouble("Drive", 0, 0, 100, 0.01, "%");
  GetParam(kLimiterRelease)->InitDouble("Limit Release", 1000, 200, 5000, 0.01, "ms", 0, "", IParam::ShapePowCurve(3));
  GetParam(kMasterInput)->InitDouble("Master Input", 100., 0., 120., 0.01, "%");
  GetParam(kMasterOutput)->InitDouble("Master Output", 100., 0., 120., 0.01, "%");


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
      HideGroup(3);
      openGroup = 1;
    };

    auto openAdvanced = [pGraphics, this, closeAdvanced](IControl* pCaller) mutable {
      ShowGroup(3);
      openGroup = 3;
    };

    auto hideHome = [pGraphics, this, closeAdvanced](IControl* pCaller) mutable {
      HideGroup(1);
    };

    auto showHome = [pGraphics, this, closeAdvanced](IControl* pCaller) mutable {
      ShowGroup(1);
    };

    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    b = pGraphics->GetBounds();

 
    controls.resize(0);
    pGraphics->AttachControl(new SSkin(IRECT(0, 0, PLUG_WIDTH, PLUG_HEIGHT), 0));
    SKnob_Style style1 = SKnob_Style(IColor(255,50,50,50), IColor(255,200,200,200), IColor(255,240, 240, 240), 4.f, 4.f, IColor(255,40,150,255), false);
    SKnob_Style style2 = SKnob_Style(IColor(255, 217, 217, 217), IColor(255, 38, 38, 38), IColor(255, 38, 38, 38), 4.f, 4.f, IColor(255,40,150,255), true);

    SKnob* thresholdKnob;
    SKnob* ratioKnob;
    SKnob* attackKnob;
    SKnob* releaseKnob;
    SKnob* gainOutKnob;
    SKnob* gainInKnob;

    SKnob* lowpassKnob;
    SKnob* highpassKnob;
    SSkin* advancedSkin;

    SCloseButton* closeButton;

    SCustomInterface<3, 512>* histogram;
    SRatioViewer<1, 512>* ratioViewer;
    STextButton* advancedButton;
    STextLogoButton* logoButton;

    SToggleButton* toggleBandpassCompression;
    SToggleButton* toggleClipping;
    SToggleButton* toggleTransients;

    SOptionChooser* compressType;
    SOptionChooser* clipType;

    SKnob* transientsKnob;

    SKnob* driveKnob;
    SKnob* limiterReleaseKnob;

    SKnob* masterInputKnob;
    SKnob* masterOutputKnob;

    thresholdKnob = new SKnob(b.GetCentredInside(100).GetHShifted(-400).GetVShifted(-85), kThreshold, style1);
    ratioKnob = new SKnob(b.GetCentredInside(100).GetHShifted(-300).GetVShifted(-85), kRatio, style1);
    attackKnob = new SKnob(b.GetCentredInside(100).GetHShifted(-400).GetVShifted(15), kAttack, style1);
    releaseKnob = new SKnob(b.GetCentredInside(100).GetHShifted(-300).GetVShifted(15), kRelease, style1);
    gainOutKnob = new SKnob(b.GetCentredInside(100).GetHShifted(-300).GetVShifted(115), kGainOut, style1);
    gainInKnob = new SKnob(b.GetCentredInside(100).GetHShifted(-400).GetVShifted(115), kGainIn, style1);

    histogram = new SCustomInterface<3, 512>(IRECT(460, 60, 880, 330), kThreshold);
    ratioViewer = new SRatioViewer<1, 512>(IRECT(200, 60, 440, 330), { kThreshold, kRatio });
    advancedButton = new STextButton(IRECT(795, 5, 895, 33), openAdvanced, "Advanced");
    logoButton = new STextLogoButton(IRECT(5, 0, 240, 40));

    closeButton = new SCloseButton(IRECT(865, 5, 895, 35), closeAdvanced);
    advancedSkin = new SSkin(IRECT(0, 0, PLUG_WIDTH, PLUG_HEIGHT), 1);

    toggleBandpassCompression = new SToggleButton(IRECT(20, 300, 240, 330), kBandpassCompress);
    compressType = new SOptionChooser(IRECT(20, 90, 240, 120), kCompressMode);
    toggleClipping = new SToggleButton(IRECT(450, 300, 670, 330), kClipPower);
    clipType = new SOptionChooser(IRECT(450, 90, 670, 120), kClipType);

    toggleTransients = new SToggleButton(IRECT(270, 300, 420, 330), kTransientPower);

    lowpassKnob = new SKnob(b.GetCentredInside(100).GetHShifted(-380).GetVShifted(20), kHighpassFreq, style2, 15, 10);
    highpassKnob = new SKnob(b.GetCentredInside(100).GetHShifted(-260).GetVShifted(20), kLowpassFreq, style2, 15, 10);
    transientsKnob = new SKnob(b.GetCentredInside(100).GetHShifted(-105).GetVShifted(20), kTransients, style2);


    driveKnob = new SKnob(b.GetCentredInside(100).GetHShifted(50).GetVShifted(20), kDrive, style2);
    limiterReleaseKnob = new SKnob(b.GetCentredInside(100).GetHShifted(170).GetVShifted(20), kLimiterRelease, style2);

    masterInputKnob = new SKnob(b.GetCentredInside(100).GetHShifted(350).GetVShifted(-55), kMasterInput, style2);
    masterOutputKnob = new SKnob(b.GetCentredInside(100).GetHShifted(350).GetVShifted(75), kMasterOutput, style2);

    controls.push_back(SControl(thresholdKnob, 1));
    controls.push_back(SControl(ratioKnob, 1));
    controls.push_back(SControl(attackKnob, 1));
    controls.push_back(SControl(releaseKnob, 1));
    controls.push_back(SControl(gainOutKnob, 1));
    controls.push_back(SControl(gainInKnob, 1));

    controls.push_back(SControl(histogram, 1));
    controls.push_back(SControl(ratioViewer, 1));
    controls.push_back(SControl(advancedButton, 1));
    controls.push_back(SControl(logoButton, 2));

    controls.push_back(SControl(closeButton, 3));
    controls.push_back(SControl(advancedSkin, 3));
    controls.push_back(SControl(compressType, 3));
    controls.push_back(SControl(clipType, 3));

    controls.push_back(SControl(toggleBandpassCompression, 3));
    controls.push_back(SControl(toggleClipping, 3));
    controls.push_back(SControl(lowpassKnob, 3));
    controls.push_back(SControl(highpassKnob, 3));
    controls.push_back(SControl(transientsKnob, 3));
    controls.push_back(SControl(toggleTransients, 3));

    controls.push_back(SControl(driveKnob, 3));
    controls.push_back(SControl(limiterReleaseKnob, 3, 1));

    controls.push_back(SControl(masterInputKnob, 3));
    controls.push_back(SControl(masterOutputKnob, 3));

    pGraphics->AttachControl(thresholdKnob);
    pGraphics->AttachControl(ratioKnob);
    pGraphics->AttachControl(attackKnob);
    pGraphics->AttachControl(releaseKnob);

    pGraphics->AttachControl(gainInKnob);
    pGraphics->AttachControl(gainOutKnob);

    pGraphics->AttachControl(advancedButton);

    pGraphics->AttachControl(histogram, kCustomWindowSend);
    pGraphics->AttachControl(ratioViewer);
    pGraphics->AttachControl(logoButton);
    pGraphics->AttachControl(advancedSkin);

    pGraphics->AttachControl(toggleBandpassCompression);
    
    pGraphics->AttachControl(toggleClipping);
    pGraphics->AttachControl(toggleTransients);

    pGraphics->AttachControl(closeButton, kCloseButton);
    pGraphics->AttachControl(compressType);
    pGraphics->AttachControl(clipType);
    pGraphics->AttachControl(lowpassKnob);
    pGraphics->AttachControl(highpassKnob);
    pGraphics->AttachControl(transientsKnob);
    pGraphics->AttachControl(driveKnob);
    pGraphics->AttachControl(limiterReleaseKnob);

    pGraphics->AttachControl(masterInputKnob);
    pGraphics->AttachControl(masterOutputKnob);
    HideGroup(3);
    openGroup = 1;
    

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
  uiLastChange = 0;
#endif
}

#if IPLUG_DSP
void StupidCompressor::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gainIn = GetParam(kGainIn)->Value() / 100.;
  const double gainOut = GetParam(kGainOut)->Value() / 100.;
  const bool clipPower = GetParam(kClipPower)->Value();
  float left;
  float right;
  float tempLeft;
  float tempRight;
  float beforeLeft;
  float beforeRight;
  for (int s = 0; s < nFrames; s++) {
    left = inputs[0][s] * masterGainInput;
    right = inputs[1][s] * masterGainInput;

    if (bandpassPower) {

      beforeLeft = left;
      beforeRight = right;

      lowpass.tick(split, left, right);
      left = split[0];
      right = split[2];

      highpass.tick(split, left, right);
      left = split[0];
      right = split[2];

      tempLeft = left;
      tempRight = right;
    }

    switch (compressOption) {
    case 0:

      left = (leftC.tickFeedForward(gainIn * left) * gainOut);
      right = (rightC.tickFeedForward(gainIn * right) * gainOut);
      break;

    case 1:
      left = (leftC.tickFeedback(gainIn * inputs[0][s]) * gainOut);
      right = (rightC.tickFeedback(gainIn * inputs[1][s]) * gainOut);
      break;

    case 2:
      left = (leftC.TickMovingAverage(gainIn * inputs[0][s]) * gainOut);
      right = (rightC.TickMovingAverage(gainIn * inputs[1][s]) * gainOut);
      break;
    }

    if (bandpassPower) {
      left = (beforeLeft - tempLeft) + left;
      right = (beforeRight - tempRight) + right;
    }


    if (transientsPower) {
      left = leftT.process(left);
      right = rightT.process(right);
    }

    if (clipPower) {
      switch (clipOption) {
      case 0:
        left = leftD.soft(left);
        right = rightD.soft(right);
        break;

      case 1:
        left = leftD.hard(left);
        right = rightD.hard(right);
        break;

      case 2:
        left = leftL.tick(left);
        right = rightL.tick(right);
        break;
      }
    }
    


    outputs[0][s] = left * masterGainOutput;
    outputs[1][s] = right * masterGainOutput;


    movingAverage = (movingAverage * 8000 + abs(outputs[0][s]) + abs(outputs[1][s])) / 8002;
    movingAverageInput = (movingAverageInput * 8000 + abs(inputs[0][s]) + abs(inputs[1][s])) / 8002;
    }
  
  mDisplaySender.PushData({ kCustomWindowSend, {movingAverage*2.65f, leftC.returnGain(), movingAverageInput*2.65f} });

}


void StupidCompressor::OnIdle() {
  mDisplaySender.TransmitData(*this);
}

void StupidCompressor::OnParamChangeUI(int idx, EParamSource a) {
  if (auto graphics = GetUI()) {
    GetUI()->RemoveControlWithTag(kInfoText);
    GetUI()->AttachControl(new SInfoText(IRECT(500,335,900,350), idx, (openGroup==1)), kInfoText);
    uiLastChange = idx;
  }

  switch (idx) {
  case kClipType:
    ManageSpecialCases(openGroup);
    break;
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
    leftC.setThreshold(1 - value / 100);
    rightC.setThreshold(1 - value / 100);
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

  case kBandpassCompress:
    bandpassPower = value;
    break;

  case kClipPower:
    clipPower = value;
    break;

  case kTransientPower:
    transientsPower = value;
    break;

  case kClipType:
    clipOption = value;
    break;

  case kCompressMode:
    compressOption = value;
    break;

  case kDrive:
    leftD.setDrive(value / 100);
    rightD.setDrive(value / 100);
    leftL.setDrive(value / 100);
    rightL.setDrive(value / 100);
    break;

  case kLimiterRelease:
    leftL.setRelease(value * GetSampleRate()/1000);
    rightL.setRelease(value * GetSampleRate()/1000);
    break;

  case kTransients:
    leftT.setMod(value / 100);
    rightT.setMod(value / 100);
    break;

  case kLowpassFreq:
    lowpass.set(value / GetSampleRate() * 2, 0.707, 0);
    break;

  case kHighpassFreq:
    highpass.set(value / GetSampleRate() * 2, 0.707, 1);
    break;

  case kMasterInput:
    masterGainInput = value / 100;
    break;

  case kMasterOutput:
    masterGainOutput = value / 100;
    break;
  }

}

void StupidCompressor::HideGroup(int group) {
  for (int i = 0; i < controls.size(); i++) {
    if (controls[i].group == group) {
      controls[i].control->Hide(true);
    }
  }
}

void StupidCompressor::ShowGroup(int group) {
  for (int i = 0; i < controls.size(); i++) {
    if (controls[i].group == group) {
      controls[i].control->Hide(false);

    }
  }
  ManageSpecialCases(group);
  if (auto graphics = GetUI()) {
    GetUI()->RemoveControlWithTag(kInfoText);
    GetUI()->AttachControl(new SInfoText(IRECT(500, 335, 900, 350), uiLastChange, (group == 1)), kInfoText);
  }
}


void StupidCompressor::ManageSpecialCases(int group) {
  for (int i = 0; i < controls.size(); i++) {
    if (controls[i].group == group) {

      //------------------------------Begin custom cases

      if (controls[i].specialCase == 1) {
        //Limiter release

        if (clipOption == 2) {
          controls[i].control->Hide(false);
        }
        else {
          controls[i].control->Hide(true);
        }
      }
    }
  }
}