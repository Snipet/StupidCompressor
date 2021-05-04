#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


class SOptionChooser : public ITextControl {
public:
  SOptionChooser(const IRECT& bounds, int param) : ITextControl(bounds, ""){
    SetParamIdx(param);
    mDblAsSingleClick = true;
    mDisablePrompt = false;
    mIgnoreMouse = false;
  }

  void Draw(IGraphics& g) {
    char buffer[64];
    WDL_String str;
    GetParam()->GetDisplay(str);
    sprintf(buffer, "%s: %s", GetParam()->GetName(), str.Get());
    g.FillRoundRect(IColor(255, 60, 60, 60), mRECT, 5);
    g.FillRoundRect(IColor(255, 20, 20, 20), mRECT.GetPadded(-1), 5);
    g.DrawText(IText(15, EAlign::Center, IColor(255, 240, 240, 240)), buffer, mRECT);
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod) {
    if (mod.L || mod.R)
    {
      PromptUserInput(mRECT);
    }
  }
};