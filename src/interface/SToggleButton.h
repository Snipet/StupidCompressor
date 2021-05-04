#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


class SToggleButton : public IControl {
public:
  SToggleButton(const IRECT& bounds, int param) : IControl(bounds, param) {

  }

  void Draw(IGraphics& g) {
    char buffer[64];
    g.FillRoundRect(IColor(255, 60, 60, 60), mRECT, 5);

    if (GetValue() == 0) {
      sprintf(buffer, "%s Disabled", GetParam()->GetName());
      g.FillRoundRect(IColor(255, 50, 50, 50), mRECT.GetPadded(-1), 5);
      g.DrawText(IText(15, EAlign::Center, IColor(255, 150, 150, 150)), buffer, mRECT);
    }
    else {
      sprintf(buffer, "%s Enabled", GetParam()->GetName());
      g.FillRoundRect(IColor(255, 20, 20, 20), mRECT.GetPadded(-1), 5);
      g.DrawText(IText(15, EAlign::Center, IColor(255, 245, 245, 245)), buffer, mRECT);
    }

    
    
  }

  void OnMouseUp(float x, float y, const IMouseMod& mod) {
    if (bounds_math::mouseInBounds(mRECT, x, y)) {
      if (GetValue() == 0) {
        SetValue(1);
      }
      else {
        SetValue(0);
      }
      SetDirty();
    }
  }
};