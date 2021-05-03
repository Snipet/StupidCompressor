#pragma once
#include "IControl.h"
#include "../common.h"
#include "config.h" //LOGO_FN was giving me errors and this fixed it
using namespace iplug;
using namespace igraphics;

//Bounds should be the entire plugin
class SAboutWindow : public IControl {
public:
  SAboutWindow(const IRECT& bounds) : IControl(bounds) {

  }

  void Draw(IGraphics& g) {
    g.FillRect(IColor(120, 0, 0, 0), mRECT);
    g.FillRect(IColor(255, 40, 40, 40), mRECT.GetPadded(-300, -30, -300, -30));
    IBitmap b = g.LoadBitmap(LOGO_FN, 1, true);
    g.DrawBitmap(b, mRECT.GetPadded(-300, -30, -300, -30).GetCentredInside(250).GetHShifted(25));

    char version[32];
    sprintf(version, "v%s", PLUG_VERSION_STR);
    g.DrawText(IText(25, EAlign::Center, IColor(255, 250, 250, 250)), version, mRECT.GetPadded(-300, -30, -300, -30).GetCentredInside(250).GetVShifted(120).GetHShifted(0));
  }


  void OnMouseDown(float x, float y, const IMouseMod& mod) {
    GetUI()->RemoveControlWithTag(kAboutButton);
  }
};