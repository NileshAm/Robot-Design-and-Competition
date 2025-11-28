#pragma once
#include <Arduino.h>

class UpdatePID {
public:
    UpdatePID(Stream &serial); // use HardwareSerial or SoftwareSerial
    void update();          // call in loop()
    float getKp() const;
    float getKi() const;
    float getKd() const;
    void parseCommand(const String &cmd);
    bool isUpdated() const;
    void resetUpdated();
    int getType() const;

private:
    Stream &_serial;
    float _Kp, _Ki, _Kd;
    int _type;
    bool _updated;
    String _buffer;

    // void parseCommand(const String &cmd);
};
