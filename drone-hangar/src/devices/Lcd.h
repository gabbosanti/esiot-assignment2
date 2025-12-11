#ifndef __LCD__
#define __LCD__

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "config.h"

class Lcd {
public:

  Lcd(uint8_t addr, uint8_t cols, uint8_t rows);

  void init();  // inizializza LCD
  void clear(); // pulisce lo schermo
  void print(const char* text);
  void printAt(uint8_t col, uint8_t row, const char* text);
  void setCursor(uint8_t col, uint8_t row);

protected:
  LiquidCrystal_I2C lcd;
  uint8_t cols;
  uint8_t rows;
};

#endif