#pragma once
#include <Arduino.h>
#include <Robot.h>

class UpdatePID {
public:
    UpdatePID(Stream &serial);
    void update(Robot& robot);

private:
    Stream &_serial;
    String _buffer;
    
    void parseCommand(const String &cmd, Robot& robot);
};
