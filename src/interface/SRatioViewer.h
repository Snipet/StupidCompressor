#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;


template <int MAXNC = 1, int dx = 256>
class SRatioViewer : public IControl {
public:
  SRatioViewer(const IRECT& bounds, const std::initializer_list<int>& params) : IControl(bounds, params) {
    bound = mRECT.GetPadded(-4);
    window = mRECT.GetPadded(-4, -mRECT.H() * 0.1, -4, -4);
    memset(points, 0, sizeof(points));
    std::vector<int> tempParams = params;
    thresholdID = tempParams[0];
    ratioID = tempParams[1];
  }

  void Draw(IGraphics& g) {
    updatePoints();
    g.FillRoundRect(IColor(255, 50, 50, 50), mRECT, 5);
    g.FillRoundRect(IColor(255, 230, 230, 230), bound, 5);
    DrawData(g, points, dx, window, 0);
    g.FillRoundRect(IColor(255, 50, 50, 50), mRECT.GetPadded(0, 0, 0, -mRECT.H() / 1.1), 5, 5, 0, 0);
  }


  void OnMouseDown(float x, float y, const IMouseMod& mod) {

  }

  void OnMouseDblClick(float x, float y, const IMouseMod& mod) {

  }

  void updatePoints() {
    float x = 0;
    float inc = window.W() / (float)dx / window.W();
    float ratio = GetParam(1)->Value();
    float threshold = GetParam(0)->Value();
    for (int i = 0; i < dx; i++) {
      if (x < threshold) {
        points[i] = x;
      }
      else {
        points[i] = ((1 / ratio) * x) - ((1 / ratio) / (1 / threshold) - threshold);
      }
      x += inc;
    }
  }

  void OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) override {
    if (!IsDisabled() && msgTag == ISender<>::kUpdateMessage) {
      IByteStream stream(pData, dataSize);

      int pos = 0;
      ISenderData<MAXNC> d;
      pos = stream.Get(&d, pos);
      updatePoints();
      SetDirty(false);
    }

  }


  //Draws array containing values from 0 ... 1
  void DrawData(IGraphics& g, float* data, int size, IRECT b, int beginning = 0) {
    float inc = b.W() / ((float)size - 1);
    float x = b.L;
    g.PathMoveTo(b.L, b.B);
    g.PathLineTo(x, getYPos(data[beginning % size], b));
    for (int i = 1; i < size; i++) {
      x += inc;
      g.PathLineTo(x, getYPos(data[(i + beginning) % size], b));
    }
    g.PathLineTo(x, getYPos(data[(size - 1 + beginning) % size], b));
    g.PathLineTo(b.R, b.B);
    g.PathClose();

    IFillOptions fillOptions;
    fillOptions.mFillRule = EFillRule::Preserve;
    fillOptions.mPreserve = true;
    g.PathFill(IColor(255, 100, 150, 255), fillOptions);


    g.PathStroke(IColor(255, 50, 50, 50), 2);
  }

private:
  IRECT bound;
  IRECT window;
  float points[dx];
  int thresholdID;
  int ratioID;
  float getYPos(float value, IRECT b) {
    return (b.H() * (1 - value) + b.T);
  }
};