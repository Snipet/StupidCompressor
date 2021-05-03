#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


class SSkin : public IControl {
public:
  SSkin(const IRECT& bounds, int selection) : IControl(bounds) {
    type = selection;
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
      break;
    }

  }

private:
  int type;
};