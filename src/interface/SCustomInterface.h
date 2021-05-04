#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


template <int MAXNC = 1, int dx = 256>
class SCustomInterface : public IControl {
public:
  SCustomInterface(const IRECT& bounds, int param) : IControl(bounds, param) {
    bound = mRECT.GetPadded(-4);
    window = mRECT.GetPadded(-4, -mRECT.H() *0.1, -4, -4);
    writeIndex = 0;
    memset(points, 0, sizeof(points));
    memset(pointsInput, 0, sizeof(pointsInput));
  }

  void Draw(IGraphics& g) {

    g.FillRoundRect(IColor(255, 50, 50, 50), mRECT, 5);
    g.FillRoundRect(IColor(255, 230, 230, 230), bound, 5);
    DrawData(g, pointsInput, dx, window, (writeIndex + 1) % dx, IColor(120, 0, 255, 200), IColor(140, 20, 20, 20));
    DrawData(g, points, dx, window, (writeIndex + 1) % dx, IColor(255, 100, 150, 255), IColor(255, 50, 50, 50));
    g.FillRoundRect(IColor(255, 50, 50, 50), mRECT.GetPadded(0, 0, 0, -mRECT.H() / 1.1), 5, 5, 0, 0);
    g.DrawText(IText(21, EAlign::Center, IColor(255, 240, 240, 240)), "Compression", mRECT.GetPadded(0, 0, 0, -mRECT.H() / 1.1));

    float y1 = window.T + window.H() * (1-GetParam(0)->Value());
    float y2 = window.T + window.H() * (1-gain);
    g.DrawLine(IColor(150, 10, 100, 255), window.L, y1, window.R, y1, 0, 5);
    g.DrawLine(IColor(150, 50, 50, 50), window.L, y2, window.R, y2, 0, 5);
  }


  void OnMouseDown(float x, float y, const IMouseMod& mod) {
    
  }

  void updatePoints(float output, float input) {
    writeIndex = (writeIndex + 1) % dx;
    points[writeIndex] = output;
    pointsInput[writeIndex] = input;
  }

  void OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) override {
    if (!IsDisabled() && msgTag == ISender<>::kUpdateMessage) {
      IByteStream stream(pData, dataSize);

      int pos = 0;
      ISenderData<MAXNC> d;
      pos = stream.Get(&d, pos);
      updatePoints(d.vals[0], d.vals[2]);
      gain = d.vals[1];
      SetDirty(false);
    }

  }


  //Draws array containing values from 0 ... 1
  void DrawData(IGraphics& g, float* data, int size, IRECT b, int beginning , IColor fill, IColor stroke) {
    float inc = b.W() / ((float)size - 1);
    float x = b.L;
    g.PathMoveTo(b.L, b.B);
    g.PathLineTo(x, getYPos(data[beginning % size], b));
    for (int i = 1; i < size; i++) {
      x += inc;
      g.PathLineTo(x, getYPos(data[(i + beginning) % size], b));
    }
    g.PathLineTo(x, getYPos(data[(size - 1 + beginning) % size ], b));
    g.PathLineTo(b.R, b.B);
    g.PathClose();

    IFillOptions fillOptions;
    fillOptions.mFillRule = EFillRule::Preserve;
    fillOptions.mPreserve = true;
    g.PathFill(fill, fillOptions);


    g.PathStroke(stroke, 2);
   
  }

private:
  IRECT bound;
  IRECT window;
  float points[dx];
  float pointsInput[dx];
  int writeIndex;
  float gain;
  float getYPos(float value, IRECT b) {
    return (b.H()* (1-value) + b.T);
  }
};