#include "Lcd.h"
#include "Arduino.h"

Lcd::Lcd(uint8_t addr, uint8_t cols, uint8_t rows) : lcd(addr, cols, rows) {   // costruttore tipico LiquidCrystal_I2C
  this->cols = LCD_COL;
  this->rows = LCD_ROW;
}

void Lcd::init() {
  Wire.begin();            
  lcd.init();                 
  lcd.backlight();
  lcd.clear();
}

void Lcd::clear() {
  lcd.clear();
}

void Lcd::setCursor(uint8_t col, uint8_t row) {
  lcd.setCursor(col, row);
}

void Lcd::print(const char* text) {
  lcd.print(text);
}

void Lcd::printAt(uint8_t col, uint8_t row, const char* text) {
  lcd.setCursor(col, row);
  lcd.print(text);
}


