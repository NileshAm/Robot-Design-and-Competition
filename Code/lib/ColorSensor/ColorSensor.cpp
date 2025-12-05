// ColorSensor.cpp
#include "ColorSensor.h"
#define EEPROM_COLOR_START_ADDR 300

ColorSensor::ColorSensor(uint8_t s0Pin, uint8_t s1Pin, uint8_t s2Pin, uint8_t s3Pin, uint8_t outPin, uint8_t oePin)
: _s0(s0Pin), _s1(s1Pin), _s2(s2Pin), _s3(s3Pin), _out(outPin), _oe(oePin),
  _lastColor(COLOR_UNKNOWN), _stableCount(0) {
    // Initialize with some defaults
    _calRed = {0,0,0}; _calGreen = {0,0,0}; _calBlue = {0,0,0};
    _calWhite = {0,0,0}; _calBlack = {0,0,0};
}

void ColorSensor::begin() {
  pinMode(_s0, OUTPUT);
  pinMode(_s1, OUTPUT);
  pinMode(_s2, OUTPUT);
  pinMode(_s3, OUTPUT);
  pinMode(_s3, OUTPUT);
  pinMode(_out, INPUT);
  pinMode(_oe, OUTPUT);
  digitalWrite(_oe, HIGH); // Disable by default
  // Set default scaling to 20% (recommended for Arduino). If you want other scaling, call setScaling().
  digitalWrite(_s0, HIGH);
  digitalWrite(_s1, LOW);
  loadCalibration();
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
  // Enable Output
  digitalWrite(_oe, LOW);
  delayMicroseconds(100); // Allow stabilization

  // S2/S3 selection: (LL=RED, HH=GREEN, LH=BLUE) — common wiring for TCS modules
  r = _readChannel(LOW, LOW);   // red
  g = _readChannel(HIGH, HIGH); // green
  b = _readChannel(LOW, HIGH);  // blue
  
  // Disable Output
  digitalWrite(_oe, HIGH);
}

void ColorSensor::calibrateTarget(ColorName color, uint16_t samples) {
  uint32_t rSum = 0, gSum = 0, bSum = 0;
  uint32_t r, g, b;

  for (uint16_t i = 0; i < samples; i++) {
    readRaw(r, g, b);
    rSum += r;
    gSum += g;
    bSum += b;
    delay(10);
  }

  ColorReading avg = { rSum / samples, gSum / samples, bSum / samples };

  switch (color) {
    case COLOR_RED:   _calRed = avg; break;
    case COLOR_GREEN: _calGreen = avg; break;
    case COLOR_BLUE:  _calBlue = avg; break;
    case COLOR_WHITE: _calWhite = avg; break;
    case COLOR_BLACK: _calBlack = avg; break;
    default: break;
  }
}

void ColorSensor::saveCalibration() {
    int addr = EEPROM_COLOR_START_ADDR;
    EEPROM.put(addr, _calRed); addr += sizeof(ColorReading);
    EEPROM.put(addr, _calGreen); addr += sizeof(ColorReading);
    EEPROM.put(addr, _calBlue); addr += sizeof(ColorReading);
    EEPROM.put(addr, _calWhite); addr += sizeof(ColorReading);
    EEPROM.put(addr, _calBlack); addr += sizeof(ColorReading);
}

bool ColorSensor::loadCalibration() {
    int addr = EEPROM_COLOR_START_ADDR;
    EEPROM.get(addr, _calRed); addr += sizeof(ColorReading);
    EEPROM.get(addr, _calGreen); addr += sizeof(ColorReading);
    EEPROM.get(addr, _calBlue); addr += sizeof(ColorReading);
    EEPROM.get(addr, _calWhite); addr += sizeof(ColorReading);
    EEPROM.get(addr, _calBlack); addr += sizeof(ColorReading);
    return true;
}

ColorName ColorSensor::getColor() {
  uint32_t r, g, b;
  readRaw(r, g, b);

  auto dist = [&](const ColorReading& target) -> uint32_t {
      long dr = (long)r - (long)target.r;
      long dg = (long)g - (long)target.g;
      long db = (long)b - (long)target.b;
      return (uint32_t)(dr*dr + dg*dg + db*db);
  };

  uint32_t dRed = dist(_calRed);
  uint32_t dGreen = dist(_calGreen);
  uint32_t dBlue = dist(_calBlue);
  uint32_t dWhite = dist(_calWhite);
  uint32_t dBlack = dist(_calBlack);

  uint32_t minD = dRed;
  ColorName detected = COLOR_RED;

  if (dGreen < minD) { minD = dGreen; detected = COLOR_GREEN; }
  if (dBlue < minD) { minD = dBlue; detected = COLOR_BLUE; }
  if (dWhite < minD) { minD = dWhite; detected = COLOR_WHITE; }
  if (dBlack < minD) { minD = dBlack; detected = COLOR_BLACK; }

  // Stability check
  if (detected == _lastColor) {
    _stableCount++;
  } else {
    _lastColor = detected;
    _stableCount = 0;
  }

  if (_stableCount >= 2) return detected;
  return COLOR_UNKNOWN;
}
