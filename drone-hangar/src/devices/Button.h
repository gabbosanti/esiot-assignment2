#ifndef __BUTTON__
#define __BUTTON__

class Button
{

public:
  Button(int pin);
  bool isPressed();     // Rileva se è premuto ORA
  bool isPressedEdge(); // Rileva il fronte di salita (premuto adesso, non prima)

private:
  int pin;
  bool lastState;
  unsigned long lastDebounceTime;
  static const unsigned long DEBOUNCE_DELAY = 50; // 50ms
};

#endif