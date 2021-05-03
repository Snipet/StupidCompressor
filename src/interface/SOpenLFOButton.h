#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


class SOpenLFOButton : public IButtonControlBase {
public:
  SOpenLFOButton(const IRECT& bounds, IActionFunction aF) : IButtonControlBase(bounds, aF) {


  }

  void Draw(IGraphics& g) {
    g.FillRoundRect(IColor(255, 30, 30, 30), mRECT, 5);
    g.FillRoundRect(IColor(255, 60, 60, 60), mRECT.GetPadded(-2), 5);
    g.DrawText(IText(15, EAlign::Center, IColor(255, 245, 245, 245)), "Open LFO", mRECT);
  }

};