#include "DHT11Sensor.h"
#include "Arduino.h"

DHT11Sensor::DHT11Sensor(int p) : pin(p)
{
    pinMode(pin, INPUT_PULLUP);
    lastTemperature = 0.0;
    lastHumidity = 0.0;
    lastReadValid = false;
    lastReadTime = 0;
}

float DHT11Sensor::getTemperature()
{
    if (millis() - lastReadTime >= MIN_READ_INTERVAL)
    {
        readSensor();
    }
    return lastTemperature;
}

float DHT11Sensor::getHumidity()
{
    if (millis() - lastReadTime >= MIN_READ_INTERVAL)
    {
        readSensor();
    }
    return lastHumidity;
}

bool DHT11Sensor::isValid()
{
    return lastReadValid;
}

bool DHT11Sensor::readSensor()
{
    uint8_t data[5] = {0, 0, 0, 0, 0};

    // Invia segnale di start
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    delay(18);
    digitalWrite(pin, HIGH);
    delayMicroseconds(40);
    pinMode(pin, INPUT_PULLUP);

    // Attendi risposta del sensore (LOW per 80us, poi HIGH per 80us)
    if (!waitForPulse(LOW, 100))
    {
        lastReadValid = false;
        return false;
    }
    if (!waitForPulse(HIGH, 100))
    {
        lastReadValid = false;
        return false;
    }
    if (!waitForPulse(LOW, 100))
    {
        lastReadValid = false;
        return false;
    }

    // Leggi 40 bit di dati (5 byte)
    for (int i = 0; i < 5; i++)
    {
        if (!readByte(data[i]))
        {
            lastReadValid = false;
            return false;
        }
    }

    // Verifica checksum
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4])
    {
        lastReadValid = false;
        return false;
    }

    // Estrai temperatura e umidità
    lastHumidity = data[0];
    lastTemperature = data[2];

    lastReadTime = millis();
    lastReadValid = true;
    return true;
}

bool DHT11Sensor::readByte(uint8_t &byte)
{
    byte = 0;

    for (int i = 0; i < 8; i++)
    {
        // Attendi inizio bit (LOW per 50us)
        if (!waitForPulse(HIGH, 100))
        {
            return false;
        }

        // Misura durata HIGH per determinare se è 0 o 1
        unsigned long startTime = micros();
        if (!waitForPulse(LOW, 100))
        {
            return false;
        }
        unsigned long duration = micros() - startTime;

        // Se HIGH > 40us è un '1', altrimenti '0'
        byte <<= 1;
        if (duration > 40)
        {
            byte |= 1;
        }
    }

    return true;
}

bool DHT11Sensor::waitForPulse(bool level, unsigned long timeout)
{
    unsigned long startTime = micros();
    while (digitalRead(pin) != level)
    {
        if (micros() - startTime > timeout)
        {
            return false;
        }
    }
    return true;
}