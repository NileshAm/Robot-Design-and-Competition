#include <Arduino.h>
#include <IRArray.h>
#include <Utils.h>

// 8-sensor array on A0..A7
const uint8_t PINS[8] = {A0,A1,A2,A3,A4,A5,A6,A7};
// equal weights (or set your own)
const float   W[8]    = {1,1,1,1,1,1,1,1};

IRArray ir(8, PINS, W, 0.5f);

void setup() {
    Serial.begin(115200);
    ir.init();

    // ---- collect min/max while you sweep over line/background ----
    for (int i=0; i<200; ++i) {   // ~200 samples; adjust as needed
        ir.updateSensors();         // calls readRaw() & updates min/max
        delay(5);                   // small gap between samples
    }
    ir.calibrate();               // compute scale/offset from min/max
}

void loop() {
    uint16_t raw[8];
    float    norm[8];
    bool     dig[8];

    ir.readRaw(raw);
    ir.readNormalized(norm);  // internally calls readRaw()
    ir.readDigital(dig);      // internally calls readNormalized()

    float w = ir.weightedSum();

    printArray(raw, 8, ", ");

    delay(50);
}
