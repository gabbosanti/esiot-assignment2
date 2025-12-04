#ifndef __CONFIG__
#define __CONFIG__

#define L1_PIN 8
#define L2_PIN 7 //blinking led
#define L3_PIN 13 //alarm led

#define HD_SERVO_PIN 9

#define DPD_PIR_PIN 12 //Drone Presence Detector 
#define RESET_BUTTON_PIN 6
#define TEMP_ANALOG_PIN A0

#define DDD_ECHO_PIN 5
#define DDD_TRIG_PIN 4

#define L2_BLINK 500

#define D1 50
#define D2 20

#define T1 1500
#define T2 2000
#define T3 3000
#define T4 5000

#define TEMP1 40  //pre-alarm
#define TEMP2 50  //alarm

//Hangar Door movement:

#define DOOR_MOVEMENT_TIME 3000
#define HD_OPEN 180
#define HD_CLOSE 0 


#endif
