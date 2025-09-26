// ColorSensor.cpp
#include "ColorSensor.h"

ColorSensor::ColorSensor(uint8_t s0Pin, uint8_t s1Pin, uint8_t s2Pin, uint8_t s3Pin, uint8_t outPin)
: _s0(s0Pin), _s1(s1Pin), _s2(s2Pin), _s3(s3Pin), _out(outPin),
  _rMin(UINT32_MAX), _gMin(UINT32_MAX), _bMin(UINT32_MAX),
  _rMax(0), _gMax(0), _bMax(0),
  _rSmooth(0), _gSmooth(0), _bSmooth(0),
  _lastColor(COLOR_UNKNOWN), _stableCount(0) {}

void ColorSensor::begin() {
  pinMode(_s0, OUTPUT);
  pinMode(_s1, OUTPUT);
  pinMode(_s2, OUTPUT);
  pinMode(_s3, OUTPUT);
  pinMode(_out, INPUT);
  // Set default scaling to 20% (recommended for Arduino). If you want other scaling, call setScaling().
  digitalWrite(_s0, HIGH);
  digitalWrite(_s1, LOW);
}

void ColorSensor::setScaling(bool s0, bool s1){
  digitalWrite(_s0, s0 ? HIGH : LOW);
  digitalWrite(_s1, s1 ? HIGH : LOW);
}

uint32_t ColorSensor::_readChannel(bool c2, bool c3) {
  // select filter
  digitalWrite(_s2, c2 ? HIGH : LOW);
  digitalWrite(_s3, c3 ? HIGH : LOW);

  // measure both HIGH and LOW durations to get the full period
  // timeout should be generous but not infinite
  unsigned long high = pulseIn(_out, HIGH, 200000); // 200ms timeout
  unsigned long low  = pulseIn(_out, LOW, 200000);
  if (high == 0 || low == 0) {
    // timeout — return a large number (very low intensity / invalid)
    return 1000000UL;
  }
  uint32_t period = (uint32_t)(high + low); // microseconds
  return period;
}

void ColorSensor::readRaw(uint32_t &r, uint32_t &g, uint32_t &b) {
  // S2/S3 selection: (LL=RED, HH=GREEN, LH=BLUE) — common wiring for TCS modules
  r = _readChannel(LOW, LOW);   // red
  g = _readChannel(HIGH, HIGH); // green
  b = _readChannel(LOW, HIGH);  // blue
}

void ColorSensor::calibrate(uint16_t samples, bool reset) {
  uint32_t r,g,b;
  if (reset) {
    _rMin = _gMin = _bMin = UINT32_MAX;
    _rMax = _gMax = _bMax = 0;
  }
  for (uint16_t i = 0; i < samples; ++i) {
    readRaw(r,g,b);
    _rMin = min(_rMin, r); _gMin = min(_gMin, g); _bMin = min(_bMin, b);
    _rMax = max(_rMax, r); _gMax = max(_gMax, g); _bMax = max(_bMax, b);
    delay(10); // small pause to let sensor settle; adjust as needed
  }
}

// normalize: returns 0..255 where 255 = highest intensity (inverted because lower period => higher light)
uint8_t ColorSensor::_normalize(uint32_t value, uint32_t minVal, uint32_t maxVal) {
  if (minVal == UINT32_MAX || maxVal == 0 || maxVal <= minVal) return 0;
  // clamp
  if (value < minVal) value = minVal;
  if (value > maxVal) value = maxVal;
  float n = float(maxVal - value) / float(maxVal - minVal); // 0..1 with 1 = strongest reflection
  if (n < 0.0) n = 0.0; if (n > 1.0) n = 1.0;
  return (uint8_t)round(n * 255.0);
}

ColorName ColorSensor::getColor() {
  uint32_t rRaw,gRaw,bRaw;
  readRaw(rRaw,gRaw,bRaw);

  // convert to 0..255 normalized intensities
  uint8_t rN = _normalize(rRaw, _rMin, _rMax);
  uint8_t gN = _normalize(gRaw, _gMin, _gMax);
  uint8_t bN = _normalize(bRaw, _bMin, _bMax);

  // smoothing (EMA)
  const float alpha = 0.35f;
  _rSmooth = _rSmooth * (1.0f - alpha) + rN * alpha;
  _gSmooth = _gSmooth * (1.0f - alpha) + gN * alpha;
  _bSmooth = _bSmooth * (1.0f - alpha) + bN * alpha;

  float avgBright = (_rSmooth + _gSmooth + _bSmooth) / 3.0f;

  // thresholds — tweak for your setup
  const float BLACK_THRESHOLD = 10.0f; // near zero brightness -> black
  const float WHITE_SIMILARITY = 20.0f; // components similar -> white if bright
  const float WHITE_BRIGHTNESS = 200.0f;

  // BLACK?
  if (avgBright < BLACK_THRESHOLD) {
    if (_lastColor == COLOR_BLACK) _stableCount++;
    else { _lastColor = COLOR_BLACK; _stableCount = 0; }
    if (_stableCount >= 2) return COLOR_BLACK;
    return COLOR_UNKNOWN;
  }

  // WHITE? (all components similar and bright)
  if (fabs(_rSmooth - _gSmooth) < WHITE_SIMILARITY && fabs(_gSmooth - _bSmooth) < WHITE_SIMILARITY && avgBright > WHITE_BRIGHTNESS) {
    if (_lastColor == COLOR_WHITE) _stableCount++;
    else { _lastColor = COLOR_WHITE; _stableCount = 0; }
    if (_stableCount >= 2) return COLOR_WHITE;
    return COLOR_UNKNOWN;
  }

  // find dominated component
  float mx = max(_rSmooth, max(_gSmooth, _bSmooth));
  float second = (_rSmooth==mx) ? max(_gSmooth,_bSmooth) : ((_gSmooth==mx) ? max(_rSmooth,_bSmooth) : max(_rSmooth,_gSmooth));
  const float DOM_MARGIN = 30.0f; // how much larger the dominant must be than the next best

  ColorName candidate = COLOR_UNKNOWN;
  if (mx - second > DOM_MARGIN) {
    if (mx == _rSmooth) candidate = COLOR_RED;
    else if (mx == _gSmooth) candidate = COLOR_GREEN;
    else candidate = COLOR_BLUE;
  } else {
    // fallback: compute simple Euclidean distance to pure RGB vectors
    auto dist = [](float a, float b, float c, float ta, float tb, float tc){
      return (a-ta)*(a-ta) + (b-tb)*(b-tb) + (c-tc)*(c-tc);
    };
    float dr = dist(_rSmooth,_gSmooth,_bSmooth, 255,0,0);
    float dg = dist(_rSmooth,_gSmooth,_bSmooth, 0,255,0);
    float db = dist(_rSmooth,_gSmooth,_bSmooth, 0,0,255);
    if (dr < dg && dr < db) candidate = COLOR_RED;
    else if (dg < dr && dg < db) candidate = COLOR_GREEN;
    else candidate = COLOR_BLUE;
  }

  // hysteresis: require several stable reads
  if (candidate == _lastColor) _stableCount++;
  else { _lastColor = candidate; _stableCount = 0; }

  if (_stableCount >= 2) return candidate;
  return COLOR_UNKNOWN;
}
