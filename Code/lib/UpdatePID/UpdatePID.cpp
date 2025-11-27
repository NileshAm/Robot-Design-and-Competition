#include "UpdatePID.h"

UpdatePID::UpdatePID(Stream &serial)
    : _serial(serial) {}

void UpdatePID::update(Robot& robot) {
    while (_serial.available()) {
        char c = _serial.read();
        if (c == '\n') {
            parseCommand(_buffer, robot);
            _buffer = "";
        } else {
            _buffer += c;
        }
    }
}

void UpdatePID::parseCommand(const String &cmd, Robot& robot) {
    String command = cmd;
    command.trim();
    
    if (command.startsWith("SET")) {
        // SET <ID> <P> <I> <D>
        int firstSpace = command.indexOf(' ');
        int secondSpace = command.indexOf(' ', firstSpace + 1);
        int thirdSpace = command.indexOf(' ', secondSpace + 1);
        int fourthSpace = command.indexOf(' ', thirdSpace + 1);
        
        if (firstSpace != -1 && secondSpace != -1 && thirdSpace != -1 && fourthSpace != -1) {
            int id = command.substring(firstSpace + 1, secondSpace).toInt();
            float p = command.substring(secondSpace + 1, thirdSpace).toFloat();
            float i = command.substring(thirdSpace + 1, fourthSpace).toFloat();
            float d = command.substring(fourthSpace + 1).toFloat();
            
            robot.setPID((Robot::PIDType)id, p, i, d);
            _serial.println("OK");
        }
    } else if (command.startsWith("GET")) {
        // GET <ID>
        int firstSpace = command.indexOf(' ');
        if (firstSpace != -1) {
            int id = command.substring(firstSpace + 1).toInt();
            float p, i, d;
            robot.getPID((Robot::PIDType)id, p, i, d);
            
            // VAL <ID> <P> <I> <D>
            _serial.print("VAL ");
            _serial.print(id); _serial.print(" ");
            _serial.print(p, 6); _serial.print(" ");
            _serial.print(i, 6); _serial.print(" ");
            _serial.println(d, 6);
        }
    }
}
