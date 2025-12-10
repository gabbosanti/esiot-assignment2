#ifndef __LCD__
#define __LCD__

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class Lcd {
public:
  // addr = indirizzo I2C (es. 0x27 o 0x3F), cols/rows = dimensioni display
  Lcd(uint8_t addr, uint8_t cols = 16, uint8_t rows = 2);

  void init();                         // inizializza LCD
  void clear();                        // pulisce
  void printAt(uint8_t col, uint8_t row, const char* text);
  void print(const char* text);
  void setCursor(uint8_t col, uint8_t row);
  void switchOn();                     // display on + backlight
  void switchOff();                    // display off + backlight off

protected:
  LiquidCrystal_I2C lcd;
  uint8_t cols;
  uint8_t rows;
};

#endif