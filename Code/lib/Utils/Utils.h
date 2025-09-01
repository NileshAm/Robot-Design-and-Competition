#pragma once
#include <Arduino.h>

// Stream any numeric array (int, long, bool, uint16_t, etc.)
// Keep the *template definition* in the header.
template <typename T>
inline void printArray(const T* a, size_t n, const char* sep = ", ") {
  if (!a) return;
  for (size_t i = 0; i < n; ++i) {
    Serial.print(a[i]);
    if (i + 1 < n) Serial.print(sep);
  }
  Serial.println();
}

// Declare the float-specific helper here (with defaults, if you want).
void printArrayF(const float* a, size_t n, const char* sep = ", ", uint8_t digits = 2);
