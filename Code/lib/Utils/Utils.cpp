#include "Utils.h"

// Definition must NOT repeat default args.
void printArrayF(const float* a, size_t n, const char* sep, uint8_t digits) {
  if (!a) return;
  for (size_t i = 0; i < n; ++i) {
    Serial.print(a[i], digits);
    if (i + 1 < n) Serial.print(sep);
  }
  Serial.println();
}
