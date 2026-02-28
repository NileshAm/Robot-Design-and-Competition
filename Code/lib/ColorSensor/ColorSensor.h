// ColorSensor.h
#ifndef COLORSENSOR_H
#define COLORSENSOR_H
#include <Arduino.h>
#include <EEPROM.h>

enum ColorName { COLOR_UNKNOWN=0, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_BLACK, COLOR_WHITE };

struct ColorReading {
  uint32_t r, g, b;
};

class ColorSensor {
public:
  ColorSensor(uint8_t s0Pin, uint8_t s1Pin, uint8_t s2Pin, uint8_t s3Pin, uint8_t outPin, uint8_t oePin);
  void begin(); // sets pins (default: 20% scaling)
  void setScaling(bool s0, bool s1); // manual control if needed
  void readRaw(uint32_t &r, uint32_t &g, uint32_t &b); // raw period (microseconds)
  
  // New calibration methods
  void calibrateTarget(ColorName color, uint16_t samples);
  void saveCalibration();
  bool loadCalibration();

  ColorName getColor(); // returns stable color using nearest neighbor

private:
  uint8_t _s0,_s1,_s2,_s3,_out,_oe;
  
  // Calibration data points
  ColorReading _calRed;
  ColorReading _calGreen;
  ColorReading _calBlue;
  ColorReading _calWhite;
  ColorReading _calBlack;

  ColorName _lastColor;
  uint8_t _stableCount;

  uint32_t _readChannel(bool c2, bool c3); // return period (us) = HIGH + LOW
};

#endif
