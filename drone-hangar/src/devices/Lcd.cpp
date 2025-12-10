#include "Lcd.h"
#include "Arduino.h"

Lcd::Lcd(uint8_t addr, uint8_t cols, uint8_t rows) : lcd(addr, cols, rows) {   // costruttore tipico LiquidCrystal_I2C
  this->cols = cols;
  this->rows = rows;
}

void Lcd::init() {
  Wire.begin();               // usa i pin SDA/SCL di default della board [web:44]
  lcd.init();                 // inizializza il modulo I2C [web:44]
  lcd.backlight();
  lcd.clear();
}

void Lcd::clear() {
  lcd.clear();
}

void Lcd::setCursor(uint8_t col, uint8_t row) {
  lcd.setCursor(col, row);
}

void Lcd::printAt(uint8_t col, uint8_t row, const char* text) {
  lcd.setCursor(col, row);
  lcd.print(text);
}

void Lcd::print(const char* text) {
  lcd.print(text);
}

void Lcd::switchOn() {
  lcd.display();
  lcd.backlight();
}

void Lcd::switchOff() {
  lcd.noDisplay();
  lcd.noBacklight();
}
