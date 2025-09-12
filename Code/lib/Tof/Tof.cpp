#include "Tof.h"

Tof::Tof(uint8_t xshut, int address, uint8_t sda, uint8_t scl)
{
    _xshut = xshut;
    _address = address;
    _sda = sda;
    _scl = scl;

    pinMode(_xshut, OUTPUT);
    Adafruit_VL6180X _sensor = Adafruit_VL6180X();
    
};

void Tof::disable()
{
    digitalWrite(_xshut, LOW);
    delay(10);
};

void Tof::enable()
{
    digitalWrite(_xshut, HIGH);
};

void Tof::init(int offset)
{
    digitalWrite(_xshut, HIGH);
    delay(10);
    if(!_sensor.begin()){
        Serial.println("Tof failed to initialize");
        while (1);
    }
    _sensor.setAddress(_address);
    delay(10);
    _sensor.setOffset(offset);
};

int Tof::readRange()
{
    return _sensor.readRange();
}

int Tof::readState(){
    return _sensor.readRangeStatus();
}