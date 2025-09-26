#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLED {
public:
    OLED(uint8_t width = 128, uint8_t height = 64, int8_t reset_pin = -1);

    bool begin();                   // Initialize display
    void clear();                   // Clear screen
    void displayText(const String &text, int16_t x = 0, int16_t y = 0, uint8_t size = 1);
    void displayCenteredText(const String &text, uint8_t size = 1);
    void display();                 // Push buffer to screen
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h);

private:
    Adafruit_SSD1306 _oled;
    uint8_t _width;
    uint8_t _height;
    int8_t _resetPin;
};

#endif
