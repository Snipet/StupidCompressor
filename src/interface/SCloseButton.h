#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


class SCloseButton : public IButtonControlBase {
public:
  SCloseButton(const IRECT& bounds, IActionFunction aF) : IButtonControlBase(bounds, aF) {

  }

  void Draw(IGraphics& g) {
    g.DrawLine(IColor(255, 150, 150, 150), mRECT.L, mRECT.B, mRECT.R, mRECT.T, 0, 3.f);
    g.DrawLine(IColor(255, 150, 150, 150), mRECT.R, mRECT.B, mRECT.L, mRECT.T, 0, 3.f);
  }
};