#pragma once
#include "IControl.h"
#include "../common.h"
using namespace iplug;
using namespace igraphics;

class SKnob : public IControl {
public:
  SKnob(const IRECT& bounds, int idx, const SKnob_Style& knobStyle, float labelSize = 19, float internalTextSize = 12) : IControl(bounds, idx) {
    radius = bounds_math::getRadius(bounds)/1.8;
    style = knobStyle;
    centerx = bounds.L + (bounds.R - bounds.L) / 2;
    centery = bounds.T + (bounds.B - bounds.T) / 2;
    radiusInnerCoeff = 0.5;
    radiusOuterCoeff = 1.05;
    mAnchorAngle = -140;
    mGearing = 1.85;
    mMouseDown = false;
    specialID = idx;
    labelTextSize = labelSize;
    textSize = internalTextSize;
  }


  void Draw(IGraphics& g) {
    const IParam* pParam = GetParam();
    float angle = GetValue() * 280 - 140;
    g.FillCircle(IColor(50,0,0,0), centerx*1.01 , centery*1.01, radius);
    g.FillCircle(style.mainColor, centerx , centery, radius);
    DrawPointer(g, angle, centerx, centery);
    DrawIndicatorTrack(g, angle, centerx, centery);
    if (mMouseDown) {
      DrawLabel(g, angle, centerx, centery);
    }
    if (style.inDark) {
      g.DrawText(IText(labelTextSize, EAlign::Center, IColor(255, 220, 220, 220)), pParam->GetName(), IRECT(centerx, mRECT.B - 21, centerx, mRECT.B));
    }else{
      g.DrawText(IText(labelTextSize, EAlign::Center, IColor(255, 50, 50, 50)), pParam->GetName(), IRECT(centerx, mRECT.B - 21, centerx, mRECT.B));
    }
  }

  void DrawLabel(IGraphics& g, float angle, float cx, float cy) {

    char buffer[128];
    const IParam* pParam = GetParam();



    sprintf(buffer, "%.2f %s", pParam->Value(), pParam->GetLabel());
    if (style.inDark) {
      g.FillCircle(style.mainColor, centerx, centery, radius * 0.8);
      g.DrawArc(style.trackColor.WithContrast(-0.6).WithOpacity(0.4), cx, cy, radius * 0.8, angle >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - angle), angle >= mAnchorAngle ? angle : mAnchorAngle, &mBlend, style.trackSize / 3);
      g.DrawText(IText(textSize, EAlign::Center, style.textColor.WithContrast(-0.5).WithOpacity(0.2)), buffer, IRECT(centerx + 1, centery + 1, centerx + 1, centery + 1));
      g.DrawText(IText(textSize, EAlign::Center, style.textColor), buffer, IRECT(centerx, centery, centerx, centery));
    }
    else {
      g.FillCircle(style.mainColor.WithContrast(0.05), centerx, centery, radius * 0.8);
      g.DrawArc(style.trackColor.WithContrast(0.6).WithOpacity(0.4), cx, cy, radius * 0.8, angle >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - angle), angle >= mAnchorAngle ? angle : mAnchorAngle, &mBlend, style.trackSize / 3);
      g.DrawText(IText(textSize, EAlign::Center, style.textColor.WithContrast(-0.5).WithOpacity(0.5)), buffer, IRECT(centerx + 1, centery + 1, centerx + 1, centery + 1));
      g.DrawText(IText(textSize, EAlign::Center, style.textColor), buffer, IRECT(centerx, centery, centerx, centery));
    }

  }

  void DrawPointer(IGraphics& g, float angle, float cx, float cy)
  {
    float convert = angle / 180 * PI;
    g.DrawRadialLine(style.pointerColor, cx, cy, angle, radiusInnerCoeff * radius, radiusOuterCoeff * radius, &mBlend, style.pointerWidth);
    g.FillCircle(style.pointerColor, cx + sin(convert) * radius * radiusInnerCoeff, cy + cos(convert) * -radius * radiusInnerCoeff, style.pointerWidth / 2);
    g.FillCircle(style.pointerColor, cx + sin(convert) * radius * radiusOuterCoeff, cy + cos(convert) * -radius * radiusOuterCoeff, style.pointerWidth / 2);
  }

  void DrawIndicatorTrack(IGraphics& g, float angle, float cx, float cy)
  {
    if (style.trackSize > 0.f)
    {
      if (style.inDark) {
        g.DrawArc(style.trackColor.WithContrast(0.8).WithOpacity(0.5), cx, cy, radius * 1.3, 140 >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - 140), 140 >= mAnchorAngle ? 140 : mAnchorAngle, &mBlend, style.trackSize);
      }
      else {
        g.DrawArc(style.trackColor.WithContrast(-0.8).WithOpacity(0.5), cx, cy, radius * 1.3, 140 >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - 140), 140 >= mAnchorAngle ? 140 : mAnchorAngle, &mBlend, style.trackSize);
      }
      g.DrawArc(style.trackColor, cx, cy, radius*1.3, angle >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - angle), angle >= mAnchorAngle ? angle : mAnchorAngle, &mBlend, style.trackSize);
      g.DrawArc(style.trackColor.WithContrast(0.1), cx, cy, radius*1.3, angle >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - angle), angle >= mAnchorAngle ? angle : mAnchorAngle, &mBlend, style.trackSize/2);
    }
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod)
  {
    mMouseDown = true;
    mMouseDragValue = GetValue();

    GetUI()->HideMouseCursor(true, true);

    IControl::OnMouseDown(x, y, mod);
    SetDirty(false);
  }

  void OnMouseUp(float x, float y, const IMouseMod& mod)
  {
    GetUI()->HideMouseCursor(false);

    mMouseDown = false;
    SetDirty(false);
  }

  bool IsFineControl(const IMouseMod& mod, bool wheel) const
  {
#ifdef PROTOOLS
#ifdef OS_WIN
    return mod.C;
#else
    return wheel ? mod.C : mod.R;
#endif
#else
    return (mod.C || mod.S);
#endif
  }


  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod)
  {
    double gearing = IsFineControl(mod, false) ? mGearing * 10.0 : mGearing;

    IRECT dragBounds = mRECT;
    mMouseDragValue += static_cast<double>(dY / static_cast<double>(dragBounds.T - dragBounds.B) / gearing);
      //mMouseDragValue += static_cast<double>(dX / static_cast<double>(dragBounds.R - dragBounds.L) / gearing);

    mMouseDragValue = Clip(mMouseDragValue, 0., 1.);

    double v = mMouseDragValue;
    const IParam* pParam = GetParam();

    if (pParam && pParam->GetStepped() && pParam->GetStep() > 0)
      v = pParam->ConstrainNormalized(mMouseDragValue);

    SetValue(v);
    SetDirty();
  }


private:
  float radius;
  float centerx;
  float centery;
  float radiusInnerCoeff;
  float radiusOuterCoeff;
  float mAnchorAngle;
  bool mMouseDown;
  float mMouseDragValue;
  float mGearing;
  int specialID;
  float labelTextSize;
  float textSize;
  SKnob_Style style;
};