#include "OLED.h"

OLED::OLED(uint8_t width, uint8_t height, int8_t reset_pin)
    : _oled(width, height, &Wire, reset_pin), _width(width), _height(height), _resetPin(reset_pin) {}

bool OLED::begin() {
    Serial.println("DEBUG: Attempting to start OLED..."); // Add this

    // 1. Try Address 0x3C
    if (!_oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("DEBUG: 0x3C failed, trying 0x3D..."); // Add this
        
        // 2. Try Address 0x3D
        if (!_oled.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { 
            Serial.println("DEBUG: OLED failed completely."); // Add this
            return false;
        }
    }
    
    Serial.println("DEBUG: OLED Started Successfully!"); // Add this
    _oled.clearDisplay();
    _oled.display();
    return true;
}

void OLED::clear() {
    _oled.clearDisplay();
}

void OLED::displayText(const String &text, int16_t x, int16_t y, uint8_t size) {
    _oled.setTextSize(size);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.setCursor(x, y);
    _oled.print(text);
}

void OLED::displayCenteredText(const String &text, uint8_t size) {
    int16_t x1, y1;
    uint16_t w, h;
    _oled.setTextSize(size);
    _oled.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    int16_t x = (_width - w) / 2;
    int16_t y = (_height - h) / 2;
    _oled.setCursor(x, y);
    _oled.setTextColor(SSD1306_WHITE);
    _oled.print(text);
}

void OLED::display() {
    _oled.display();
}

void OLED::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    _oled.drawLine(x0, y0, x1, y1, SSD1306_WHITE);
}

void OLED::drawRect(int16_t x, int16_t y, int16_t w, int16_t h) {
    _oled.drawRect(x, y, w, h, SSD1306_WHITE);
}
