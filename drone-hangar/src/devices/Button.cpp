#include "Button.h"
#include "Arduino.h"

Button::Button(int pin) : pin(pin), lastState(true), lastDebounceTime(0)
{
    pinMode(pin, INPUT_PULLUP); // Pull-up interno (pin HIGH quando non premuto)
}

bool Button::isPressed()
{
    return digitalRead(pin) == LOW; // Invertito: LOW = premuto
}

bool Button::isPressedEdge()
{
    bool current = (digitalRead(pin) == LOW); // Invertito

    if (current != lastState)
    {
        lastDebounceTime = millis();
    }

    bool pressed = false;
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
    {
        pressed = current && !lastState;
    }

    lastState = current;
    return pressed;
}