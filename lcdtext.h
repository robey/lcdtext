#pragma once

#include <Arduino.h>

class LcdText {
private:
  static const uint8_t SCREEN_WIDTH = 131;
  static const uint8_t SCREEN_HEIGHT = 132;
  static const uint16_t SCREEN_PIXELS = (uint16_t)SCREEN_WIDTH * (uint16_t)SCREEN_HEIGHT;
  static const uint8_t SCREEN_X_OFFSET = 2;
  static const uint8_t SCREEN_Y_OFFSET = 2;

  static const int RESET = 8;

  // fields for SPI control register: 4MHz, MSB, falling-edge clock trigger.
  static const int SPI_ON = 0x58;
  static const int SPI_OFF = 0x18;

// in theory, there are two different drivers. in reality, i don't care.
#ifdef LCD_DRIVER_EPSON
  static const uint8_t SetPage = 0x75;
  static const uint8_t SetColumn = 0x15;
  static const uint8_t WriteData = 0x5C;
  static const uint8_t Nop = 0x25;
#else /* normal phillips driver */
  static const uint8_t SetPage = 0x2B;
  static const uint8_t SetColumn = 0x2A;
  static const uint8_t WriteData = 0x2C;
  static const uint8_t Nop = 0x00;
  static const uint8_t SleepOut = 0x11;
  static const uint8_t BoosterOn = 0x03;
  static const uint8_t ColorMode = 0x3A;
  static const uint8_t MemoryAccess = 0x36;
  static const uint8_t Contrast = 0x25;
  static const uint8_t DisplayOn = 0x29;
#endif

  int cursor;

  // color is, confusingly, in 0BGR format.
  uint16_t fgcolor;
  uint16_t bgcolor;
  uint8_t colorCache[16];

  void send(uint8_t is_command, uint8_t data);
  void cacheColors();

public:
  LcdText();
  void init();
  void sendData(uint8_t data) { send(0, data); }
  void sendCommand(uint8_t data) { send(1, data); }
  void clear();
  void drawChar(uint8_t c, uint8_t x, uint8_t y);
  void drawText(const char *s);
  void setColor(int color) { fgcolor = color; cacheColors(); }
  void setBackgroundColor(int color) { bgcolor = color; cacheColors(); }
  void setCursor(uint8_t y, uint8_t x) { cursor = (y << 5) | x; }
};
