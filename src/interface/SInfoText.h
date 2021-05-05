#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


class SInfoText : public IControl {
public:
  SInfoText(const IRECT& bounds, int param, bool bright) : IControl(bounds) {
    idx = param;
    light = bright;
  }

  void Draw(IGraphics& g) {
    char buffer[128];

    switch (idx) {
    case kParamLFORateTempo:
      sprintf(buffer, "%s %.2f %s", g.GetDelegate()->GetParam(idx)->GetName(), scalars[g.GetDelegate()->GetParam(idx)->Int()], g.GetDelegate()->GetParam(idx)->GetLabel());
      break;
    default:
      sprintf(buffer, "%s %.2f %s", g.GetDelegate()->GetParam(idx)->GetName(), g.GetDelegate()->GetParam(idx)->Value(), g.GetDelegate()->GetParam(idx)->GetLabel());
      break;
    }
    if (light) {
      g.DrawText(IText(19, EAlign::Far, IColor(255, 50, 50, 50)), buffer, mRECT);
    }
    else {
      g.DrawText(IText(19, EAlign::Far, IColor(255, 230, 230, 230)), buffer, mRECT);
    }
  }

private:
  int idx;
  bool light;
};