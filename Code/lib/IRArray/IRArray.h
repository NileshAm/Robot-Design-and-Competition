#pragma once
#include <Arduino.h>
#include <EEPROM.h>

class IRArray {
public:
  // If weights == nullptr, equal weights (1.0) are used.
  IRArray(uint8_t numSensors, const uint8_t* sensorPins,
          const float* weights = nullptr, float threshold = 0.5f);

  ~IRArray();

  void init();                          // pinMode(INPUT)

  // ---- reads (caller provides arrays of size _n) ----
  void readRaw(int* out);
  void readNormalized(double* out);      // uses readRaw()
  void digitalRead(bool* out);          // uses readNormalized()

  
  void setScalingFactor(const double* in);
  void setOffset(const double* in);
  double* getScalingFactor();
  double* getOffset();


  // ---- calibration flow ----
  // Call repeatedly while moving sensors over line/background:
  void updateSensors();                 // calls readRaw() and updates min/max
  // When done collecting extremes, compute scale/offset (from min/max):
  void calibrate();                     // sets _scale/_offset for normalization

  // ---- simple aggregate ----
  float weightedSum();                  // thresholds + weights → 0..1


private:
  uint8_t  _n;
  float    _threshold;
  float    _weightSum;

  // owned copies
  uint8_t* _pins;
  float*   _weights;

  // calibration state
  int* _minV;   // start 1023
  int* _maxV;   // start 0
  double*    _scale;  // 1/(max-min) (guarded)
  double*    _offset; // min/(max-min)

  // non-copyable
  IRArray(const IRArray&) = delete;
  IRArray& operator=(const IRArray&) = delete;
};
