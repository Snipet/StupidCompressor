#pragma once

#include "IControl.h"
#include "cmath"
using namespace iplug;
using namespace igraphics;

#define LFO_TEMPODIV_VALIST "1/64", "1/32", "1/16T", "1/16", "1/16D", "1/8T", "1/8", "1/8D", "1/4", "1/4D", "1/2", "1/1", "2/1", "4/1", "8/1"



enum ETempoDivison
{
  k64th = 0,   // 1 sixty fourth of a beat
  k32nd,       // 1 thirty second of a beat
  k16thT,      // 1 sixteenth note triplet
  k16th,       // 1 sixteenth note
  k16thD,      // 1 dotted sixteenth note
  k8thT,       // 1 eighth note triplet
  k8th,        // 1 eighth note
  k8thD,       // 1 dotted eighth note
  k4th,        // 1 quater note a.k.a 1 beat @ 4/4
  k4thD,       // 1 dotted beat @ 4/4
  k2th,        // 2 beats @ 4/4
  k1,          // 1 bar @ 4/4
  k2,          // 2 bars @ 4/4
  k4,          // 4 bars @ 4/4
  k8,          // 8 bars @ 4/4
  kNumDivisions
};


struct SControl {
  IControl* control;
  int group;
  int specialCase;
  SControl(IControl* c, int g, int sc = 0) {
    control = c;
    group = g;
    specialCase = sc;
  }
  SControl() {

  }
};
enum kLFOMessageActions {
  kUpdatePoint = 0,
  kAddPoint,
  kRemovePoint,
  kSendPoints
};

static constexpr float scalars[kNumDivisions] = {
  64. / 4., // k64th = 0,   // 1 sixty fourth of a beat
  32. / 4., // k32nd,       // 1 thirty second of a beat
  24. / 4., // k16thT,      // 1 sixteenth note triplet
  16. / 4., // k16th,       // 1 sixteenth note
  12. / 4., // k16thD,      // 1 dotted sixteenth note
  9. / 4., // k8thT,       // 1 eighth note triplet
  8. / 4., // k8th,        // 1 eighth note
  6 / 4., // k8thD,       // 1 dotted eighth note
  4. / 4., // k4th,        // 1 quater note a.k.a 1 beat @ 4/4
  3. / 4., // k4thD,       // 1 dotted beat @ 4/4
  2. / 4., // k2th,        // 2 beats @ 4/4
  1. / 4., // k1,          // 1 bar @ 4/4
  0.5 / 4., // k2,          // 2 bars @ 4/4
  0.25 / 4., // k4,          // 4 bars @ 4/4
  0.125 / 4., // k8,          // 8 bars @ 4/4
};


struct SKnob_Style {
  IColor mainColor;
  IColor pointerColor;
  float pointerWidth;
  float trackSize;
  IColor textColor;
  IColor trackColor;
  bool inDark;
  SKnob_Style(const IColor& m, const IColor& p, const IColor& tc = IColor(255, 240, 240, 240), float pw = 4.f, float ts = 4.f, const IColor& tkc = COLOR_RED, bool inTheDark = false) {
    mainColor = m;
    pointerColor = p;
    textColor = tc;
    pointerWidth = pw;
    trackSize = ts;
    trackColor = tkc;
    inDark = inTheDark;
  }
  SKnob_Style() {

  }
};

namespace bounds_math {

  float getRadius(IRECT bounds) {
    return ((bounds.B - bounds.T) + (bounds.R - bounds.L)) / 4;
  }

  bool mouseInBounds(IRECT bounds, float x, float y) {
    return ((x > bounds.L && x < bounds.R) && (y > bounds.T && y < bounds.B));
  }

  IRECT coordsToIRECT(float x, float y) {
    return IRECT(x, y, x, y);
  }

  float getCenter(IRECT bounds) {
    return bounds.L + (bounds.R - bounds.L) / 2;
  }
}

float Clip(float n, float lower, float upper) {
  return std::max(lower, std::min(n, upper));
}

class Point {
public:
  int uid;
  float x;
  float y;
  bool operator < (const Point& point) const { return (this->x < point.x); };
  bool operator >(const Point& point) const { return (this->x > point.x); };
  bool operator == (const Point& point) const { return (this->uid == point.uid); };
};
