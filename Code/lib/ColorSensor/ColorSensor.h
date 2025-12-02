// ColorSensor.h
#ifndef COLORSENSOR_H
#define COLORSENSOR_H
#include <Arduino.h>
#include <EEPROM.h>

enum ColorName { COLOR_UNKNOWN=0, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_BLACK, COLOR_WHITE };

class ColorSensor {
public:
  ColorSensor(uint8_t s0Pin, uint8_t s1Pin, uint8_t s2Pin, uint8_t s3Pin, uint8_t outPin, uint8_t oePin);
  void begin(); // sets pins (default: 20% scaling)
  void setScaling(bool s0, bool s1); // manual control if needed
  void readRaw(uint32_t &r, uint32_t &g, uint32_t &b); // raw period (microseconds)
  void calibrate(uint16_t samples, bool reset = true); // reset=false keeps previous min/max
  ColorName getColor(); // returns stable color or COLOR_UNKNOWN until stable

  void saveCalibration();
  void loadCalibration();

private:
  uint8_t _s0,_s1,_s2,_s3,_out,_oe;
  uint32_t _rMin,_gMin,_bMin, _rMax,_gMax,_bMax;
  float _rSmooth,_gSmooth,_bSmooth;
  ColorName _lastColor;
  uint8_t _stableCount;

  uint32_t _readChannel(bool c2, bool c3); // return period (us) = HIGH + LOW
  uint8_t _normalize(uint32_t value, uint32_t minVal, uint32_t maxVal); // 0..255 (0 = darkest/reflectionlowest)
};

#endif
