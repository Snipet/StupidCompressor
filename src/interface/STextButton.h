#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


class STextButton : public IButtonControlBase {
public:
  STextButton(const IRECT& bounds, IActionFunction aF, char* text, float textSize = 24) : IButtonControlBase(bounds, aF) {
    label = text;
    size = textSize;
  }

  void Draw(IGraphics& g) {
    g.DrawText(IText(size, EAlign::Center, IColor(255, 245, 245, 245)), label, mRECT);
  }

private:
  char* label;
  float size;
};