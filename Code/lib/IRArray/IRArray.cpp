#include "IRArray.h"
#include <Utils.h>

IRArray::IRArray(uint8_t n, const uint8_t* pins,
                 const float* weights, float threshold)
: _n(n), _threshold(threshold), _weightSum(0.0f),
  _pins(nullptr), _weights(nullptr),
  _minV(nullptr), _maxV(nullptr), _scale(nullptr), _offset(nullptr)
{
    _pins    = new uint8_t [_n];
    _weights = new float   [_n];
    _minV    = new int[_n];
    _maxV    = new int[_n];
    _scale   = new double   [_n];
    _offset  = new double   [_n];

    for (uint8_t i=0;i<_n;++i){
        _pins[i] = pins[i];
        float w = weights ? weights[i] : 1.0f;
        _weights[i] = w;
        _weightSum += w;

        _minV[i] = 1023;
        _maxV[i] = 0;
        _scale[i] = 1.0f;
        _offset[i]= 0.0f;
    }
    if (_weightSum <= 0.0f) _weightSum = 1.0f; // avoid /0
}

IRArray::~IRArray() {
    delete[] _pins;    delete[] _weights;
    delete[] _minV;    delete[] _maxV;
    delete[] _scale;   delete[] _offset;
}

void IRArray::init() {
    for (uint8_t i=0;i<_n;++i) pinMode(_pins[i], INPUT);
}

void IRArray::readRaw(int* out) {
    for (uint8_t i=0;i<_n;++i) out[i] = analogRead(_pins[i]);
}

void IRArray::updateSensors() {
    // read raw, then update min/max
    int* raw = new int[_n];
    readRaw(raw);
    printArray(raw, 8);
    for (uint8_t i=0;i<_n;++i){
        if (raw[i] > _maxV[i]) _maxV[i] = raw[i];
        if (raw[i] < _minV[i]) _minV[i] = raw[i];
    }
    delete[] raw;
}

void IRArray::calibrate() {
    // derive scale/offset from collected min/max
    for (uint8_t i=0;i<_n;++i){
        uint16_t range = (_maxV[i] > _minV[i]) ? (uint16_t)(_maxV[i] - _minV[i]) : 1;
        _scale[i]  = 1.0f / (float)range;
        _offset[i] = _scale[i] * (float)_minV[i];
    }
}

void IRArray::readNormalized(double* out) {
    // use readRaw()
    int* raw = new int[_n];
    readRaw(raw);
    for (uint8_t i=0;i<_n;++i){
        float x = _scale[i] * (float)raw[i] - _offset[i]; // (raw-min)/(max-min)
        if (x < 0.0f) x = 0.0f; else if (x > 1.0f) x = 1.0f;
        out[i] = x;
    }
    delete[] raw;
}

void IRArray::digitalRead(bool* out) {
    // use readNormalized()
    double* norm = new double[_n];
    readNormalized(norm);
    for (uint8_t i=0;i<_n;++i) out[i] = (norm[i] <= _threshold);
    delete[] norm;
}

float IRArray::weightedSum() {
    bool* dig = new bool[_n];
    digitalRead(dig);
    float acc = 0.0f;
    for (uint8_t i=0;i<_n;++i) if (dig[i]) acc += _weights[i];
    delete[] dig;
    return acc;
}

void IRArray::setScalingFactor(const double* in) {
  for (uint8_t i = 0; i < _n; ++i) _scale[i] = in[i];
}

void IRArray::setOffset(const double* in) {
  for (uint8_t i = 0; i < _n; ++i) _offset[i] = in[i];
}
double* IRArray::getScalingFactor(){
    return _scale;
}
double* IRArray::getOffset(){
    return _offset;
}
