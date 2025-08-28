#pragma once
#include <Arduino.h>

// Stream any numeric array (int, long, bool, etc.)
template <typename T>
inline void printArray(const T* a, size_t n, const char* sep = ", ") {
    if (!a) return;
    for (size_t i = 0; i < n; ++i) {
        Serial.print(a[i]);
        if (i + 1 < n) Serial.print(sep);
    }
    Serial.println();
}

// Stream float array with fixed decimal digits
void printArrayF(const float* a, size_t n, const char* sep, uint8_t digits) {
    if (!a) return;
    for (size_t i = 0; i < n; ++i) {
        Serial.print(a[i], digits);
        if (i + 1 < n) Serial.print(sep);
    }
    Serial.println();
}
