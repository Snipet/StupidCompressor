#pragma once

#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


//Class for rendering the background of the plugin because bitmap backgrounds are for losers
class SSidebar : public IControl {
public:
  SSidebar(const IRECT& bounds) : IControl(bounds) {

  }

  void Draw(IGraphics& g) {
    g.FillRoundRect(IColor(255, 130, 130, 150), mRECT, 12, 0, 12, 0);
  }

};