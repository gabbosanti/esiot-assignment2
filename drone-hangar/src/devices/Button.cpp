#include "Button.h"
#include "Arduino.h"

Button::Button(int pin) : pin(pin), lastState(false), lastDebounceTime(0)
{
    pinMode(pin, INPUT_PULLUP);
}

bool Button::isPressed()
{
    return digitalRead(pin) == LOW;
}

bool Button::isPressedEdge()
{
    bool currentReading = (digitalRead(pin) == LOW);
    bool pressed = false;

    // Se la lettura è diversa dall'ultimo stato stabile
    if (currentReading != lastState)
    {
        lastDebounceTime = millis();
    }

    // Se è passato abbastanza tempo (stato stabile)
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY)
    {
        // Rileva il fronte: ora premuto, prima non premuto
        if (currentReading && !lastState)
        {
            pressed = true;
        }

        // Aggiorna lo stato solo dopo il debounce
        lastState = currentReading;
    }

    return pressed;
}