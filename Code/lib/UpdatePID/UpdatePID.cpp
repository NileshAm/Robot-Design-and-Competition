#include "UpdatePID.h"

UpdatePID::UpdatePID(Stream &serial)
    : _serial(serial), _Kp(0), _Ki(0), _Kd(0), _updated(false) {}

void UpdatePID::update() {
    while (_serial.available()) {
        char c = _serial.read();
        if (c == '\n') {
            parseCommand(_buffer);
            _buffer = "";
        } else {
            _buffer += c;
        }
    }
}

void UpdatePID::parseCommand(const String &cmd) {
    int kpIndex = cmd.indexOf("Kp:");
    int kiIndex = cmd.indexOf("Ki:");
    int kdIndex = cmd.indexOf("Kd:");

    if (kpIndex != -1 && kiIndex != -1 && kdIndex != -1) {
        float kp = cmd.substring(kpIndex + 3, cmd.indexOf(',', kpIndex)).toFloat();
        float ki = cmd.substring(kiIndex + 3, cmd.indexOf(',', kiIndex)).toFloat();
        float kd = cmd.substring(kdIndex + 3).toFloat();

        _Kp = kp;
        _Ki = ki;
        _Kd = kd;
        _updated = true;

        _serial.println("PID updated: ");
        _serial.print("Kp="); _serial.println(_Kp);
        _serial.print("Ki="); _serial.println(_Ki);
        _serial.print("Kd="); _serial.println(_Kd);
    }
}

float UpdatePID::getKp() const { return _Kp; }
float UpdatePID::getKi() const { return _Ki; }
float UpdatePID::getKd() const { return _Kd; }

bool UpdatePID::isUpdated() const { return _updated; }
void UpdatePID::resetUpdated() { _updated = false; }
