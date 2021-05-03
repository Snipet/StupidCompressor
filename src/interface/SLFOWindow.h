#pragma once

#include "IControl.h"
#include "../common.h"
#include "SCloseButton.h"
#include <vector>

using namespace iplug;
using namespace igraphics;



//Class for rendering the background of the plugin because bitmap backgrounds are for losers
class SLFOWindow : public IControl {
public:
  SLFOWindow(const IRECT& bounds, const std::vector<Point> input) : IControl(bounds) {
    points = input;
    dragging = false;
    pointCount = points.size();;
  }

  void Draw(IGraphics& g) {
    g.FillRect(IColor(255, 40, 40, 40), mRECT);
    for (int i = 0; i < points.size(); i++) {
      if (i > 0) {
        g.DrawLine(IColor(255, 20, 150, 255), convertToGraphicX(points[i - 1].x), convertToGraphicY(points[i - 1].y), convertToGraphicX(points[i].x), convertToGraphicY(points[i].y), 0, 5);
      }
      else {
        g.DrawLine(IColor(255, 20, 150, 255), convertToGraphicX(0), convertToGraphicY(0), convertToGraphicX(points[i].x), convertToGraphicY(points[i].y), 0, 5);

      }
    }

    if (points.size() > 0) {
      g.DrawLine(IColor(255, 20, 150, 255), convertToGraphicX(points[points.size() - 1].x), convertToGraphicY(points[points.size() - 1].y), convertToGraphicX(1), convertToGraphicY(0), 0, 5);
    }
    else {
      g.DrawLine(IColor(255, 20, 150, 255), convertToGraphicX(0), convertToGraphicY(0), convertToGraphicX(1), convertToGraphicY(0), 0, 5);
    }

    for (int i = 0; i < points.size(); i++) {
      g.FillCircle(IColor(255, 235, 235, 235), convertToGraphicX(points[i].x), convertToGraphicY(points[i].y), 5);
    }
  }

  void OnMouseDblClick(float x, float y, const IMouseMod& mod) {
    //assert(false);
    Point current = getPoint(x, y, 5);
    if (current.uid != 0) {
      //User clicked a point
      points.erase(std::remove(points.begin(), points.end(), current), points.end());
      sendPoints();
      SetDirty(false);
    }
    else {
      pointCount++;
      Point p;
      p.x = convertToPercentX(x);
      p.y = convertToPercentY(y);
      p.uid = pointCount;
      points.push_back(p);
      std::sort(points.begin(), points.end());
      sendPoints();
      SetDirty(false);
    }
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod) {
    Point current = getPoint(x, y, 5);

    if (current.uid == 0) {
      Point none;
      none.uid = 0;
      selected = none;
      dragging = false;
      SetDirty(false);
    }
    else {
      selected = current;
      dragging = true;
      SetDirty(false);
    }
  }


  void OnMouseUp(float x, float y, const IMouseMod& mod) {
    dragging = false;
    sendPoints();
  }

  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) {
    if (selected.uid == 0 || dragging == false) {
      return;
    }

    std::vector<Point>::iterator it = std::find(points.begin(), points.end(), selected);

    if (it != points.end()) {
      (&(*it))->x = Clip(convertToPercentX(x), 0, 1);
      (&(*it))->y = Clip(convertToPercentY(y), 0, 1);
      std::sort(points.begin(), points.end());
      //sendPoints();
    }
    SetDirty(false);
  }

  Point getPoint(double x, double y, double epsilon) {
    for (std::vector<Point>::iterator it = points.begin(); it != points.end(); ++it) {
      Point point = *it;
      double xGraphic = convertToGraphicX(point.x);
      double yGraphic = convertToGraphicY(point.y);

      if ((xGraphic - epsilon < x && xGraphic + epsilon > x) &&(yGraphic - epsilon < y && yGraphic + epsilon > y)) {
        return point;
      }
    }

    Point none;
    none.uid = 0;
    return none;
  }


private:
  std::vector<Point> points;
  Point selected;
  bool dragging;
  int pointCount;

  float distance(int x1, int y1, int x2, int y2)
  {
    // Calculating distance
    return sqrt(pow(x2 - x1, 2) +
      pow(y2 - y1, 2) * 1.0);
  }

  double convertToGraphicX(double value) {
    double min = (double)this->mRECT.L;
    double distance = (double)this->mRECT.W();
    return value * distance + min;
  };
  double convertToPercentX(double value) {
    double min = (double)this->mRECT.L;
    double position = value - min;
    double distance = (double)this->mRECT.W();
    return position / distance;
  };
  double convertToGraphicY(double value) {
    double min = (double)this->mRECT.T;
    double distance = (double)this->mRECT.H();
    // We use (1 - value) as the max value 1 is located on top of graphics and not bottom
    return (1 - value) * distance + min;
  };
  double convertToPercentY(double value) {
    double min = (double)this->mRECT.T;
    double position = value - min;
    double distance = (double)this->mRECT.H();
    // We return the 1 - distance as the value 1 is located on top of graphics and not bottom
    return 1 - position / distance;
  };

  void updatePoint(int index) {
    double send[4] = { index, points[index].x, points[index].y, points[index].uid };
    GetUI()->GetDelegate()->SendArbitraryMsgFromUI(kUpdatePoint, -1, sizeof(send), &send);
  }

  void addPoint(int index) {
    double send[4] = { index, points[index].x, points[index].y, points[index].uid };
    GetUI()->GetDelegate()->SendArbitraryMsgFromUI(kAddPoint, -1, sizeof(send), &send);
  }

  void deletePoint(int index) {
    double send[4] = { index, points[index].x, points[index].y, points[index].uid };
    GetUI()->GetDelegate()->SendArbitraryMsgFromUI(kRemovePoint, -1, sizeof(send), &send);
  }

  void sendPoints() {
    GetUI()->GetDelegate()->SendArbitraryMsgFromUI(kSendPoints, -1, sizeof(points), &points);

  }
};