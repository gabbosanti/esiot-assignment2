#ifndef __CONFIG__
#define __CONFIG__

// PIN DEFINITIONS

// LEDs
#define L1_PIN 11
#define L2_PIN 12 // blinking led
#define L3_PIN 13 // alarm led

// BUTTON
#define RESET_BUTTON_PIN 2 // Reset Button

// SERVO MOTOR
#define HD_SERVO_PIN 9

// SENSORS
#define DPD_PIR_PIN 10     // Drone Presence Detector (PIR)
#define TEMP_ANALOG_PIN A0 // Temperature Sensor (Analog Pin)

// ULTRASONIC SENSOR
#define DDD_ECHO_PIN 4
#define DDD_TRIG_PIN 5

// LCD
#define LCD_ROW 16
#define LCD_COL 2
#define LCD_I2C_ADDR 0x27

// TEMPERATURE SENSOR
#define TEMP_SENSOR_PIN 7

// CONFIG PARAMETERS
#define L2_BLINK 500

#define D1 0.20
#define D2 0.10

#define T1 2000
#define T2 3000
#define T3 3000
#define T4 5000

#define TEMP1 40 // pre-alarm
#define TEMP2 50 // alarm

// Hangar Door movement:

#define DOOR_MOVEMENT_TIME 3000
#define HD_OPEN 180
#define HD_CLOSE 0

#endif
