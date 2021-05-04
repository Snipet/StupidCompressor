#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


class SSkin : public IControl {
public:
  SSkin(const IRECT& bounds, int selection) : IControl(bounds) {
    type = selection;
    bandpassCompressionWindow = IRECT(10, 50, 250, 340);
    clippingWindow = IRECT(260, 50, 500, 340);
    transientWindow = IRECT(510, 50, 680, 340);
    topPadding = -262;
  }

  void Draw(IGraphics& g) {
    switch (type) {
    case 0:
      g.FillRect(IColor(255, 245, 245, 245), mRECT);
      g.FillRect(IColor(255, 30, 30, 30), mRECT.GetPadded(0, 0, 0, -310));
      break;

    case 1:
      g.FillRect(IColor(255, 40, 40, 45), mRECT);
      g.FillRect(IColor(255, 30, 30, 30), mRECT.GetPadded(0, 0, 0, -310));

      g.FillRoundRect(IColor(255, 30, 30, 30), bandpassCompressionWindow, 5);
      g.FillRoundRect(IColor(255, 22, 22, 22), bandpassCompressionWindow.GetPadded(0, 0, 0, topPadding), 5, 5, 0, 0);
      g.DrawText(IText(21, EAlign::Center, IColor(255, 230, 230, 230)), "Bandpass Compression", bandpassCompressionWindow.GetPadded(0, 0, 0, topPadding));

      g.FillRoundRect(IColor(255, 30, 30, 30), clippingWindow, 5);
      g.FillRoundRect(IColor(255, 22, 22, 22), clippingWindow.GetPadded(0, 0, 0, topPadding), 5, 5, 0, 0);
      g.DrawText(IText(21, EAlign::Center, IColor(255, 230, 230, 230)), "Clipping", clippingWindow.GetPadded(0, 0, 0, topPadding));

      g.FillRoundRect(IColor(255, 30, 30, 30), transientWindow, 5);
      g.FillRoundRect(IColor(255, 22, 22, 22), transientWindow.GetPadded(0, 0, 0, topPadding), 5, 5, 0, 0);
      g.DrawText(IText(21, EAlign::Center, IColor(255, 230, 230, 230)), "Transients", transientWindow.GetPadded(0, 0, 0, topPadding));
      break;
    }

  }

private:
  int type;
  float topPadding;
  IRECT bandpassCompressionWindow;
  IRECT clippingWindow;
  IRECT transientWindow;
};