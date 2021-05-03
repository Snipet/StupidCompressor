#pragma once

#pragma once
#include "IControl.h"
#include "../common.h"
#include "SAboutWindow.h"
using namespace iplug;
using namespace igraphics;

class STextLogoButton : public IControl {
public:
  STextLogoButton(const IRECT& bounds) : IControl(bounds) {

  }

  void Draw(IGraphics& g) {
    g.DrawText(IText(35, EAlign::Near, IColor(255, 250, 250, 250)), "stupid example", mRECT);
  }


  void OnMouseDown(float x, float y, const IMouseMod& mod) {
    GetUI()->AttachControl(new SAboutWindow(IRECT(0,0,PLUG_WIDTH, PLUG_HEIGHT)), kAboutButton);
  }
};