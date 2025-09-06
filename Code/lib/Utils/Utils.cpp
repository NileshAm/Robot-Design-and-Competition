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
// Definition must NOT repeat default args.
void printArrayD(const double* a, size_t n, const char* sep, uint8_t digits) {
  if (!a) return;
  for (size_t i = 0; i < n; ++i) {
    Serial.print(a[i], digits);
    if (i + 1 < n) Serial.print(sep);
  }
  Serial.println();
}


uint8_t pack8(const bool a[8]){
  uint8_t v = 0;
  for (uint8_t i = 0; i < 8; ++i) v |= (a[i] ? 1 : 0) << i;   // or << (7-i) if MSB-first index
  return v;
}
uint32_t pushBitMSB(uint32_t acc, uint8_t bit) {
  return (acc << 1) | (bit & 1u);
}