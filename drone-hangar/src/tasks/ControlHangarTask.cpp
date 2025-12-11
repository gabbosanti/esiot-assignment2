#include "ControlHangarTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

#define FWD_TIME 2000
#define BWD_TIME 1000
#define START_TIME 100
#define RESET_TIME 500
#define DRU_ACTIVATE 'OPEN DOOR'
#define DRU_OPENING 'DRONE ARRIVED'
ControlHangarTask::ControlHangarTask(Button* pButton, ServoMotor* pMotor, Sonar* pSonar, Pir * pPir, TempSensorTMP36* pTempSensor, Lcd* pLcd, Context* pContext): 
    pMotor(pMotor), pButton(pButton), pTempSensor(pTempSensor), pLcd(pLcd), pContext(pContext){
    setState(IDLE);
    pendingPreAlarm = false;
}
  
void ControlHangarTask::tick(){
    switch (state){    

        case IDLE: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] IDLE"));
                pLcd->print("DRONE INSIDE");
                pContext->setDisplayState(DisplayState::IDLE);
            }

            float temp = pTempSensor->getTemperature(); //temperatura rilevata dal sensore
            long dt = elapsedTimeInState(); //temD){ //Se è apo trascorso nello stato   
            if((temp >= TEMP1 && dt > T3) || pendingPreAlarm){
                setState(PRE_ALARM);
            }

            if (Serial.read() == DRU_ACTIVATE){ //Se è arrivato il comando di apertura hangar
                
                pMotor->on(); //Attiva servo
                pMotor->setPosition(HD_OPEN); //Posiziona hangar in chiuso
                pLcd->clear();
                pLcd->print("TAKEOFF");       
                pContext->setDisplayState(DisplayState::TAKEOFF);
                setState(TAKEOFF);
            }
            else{
                //Ciclo su se stesso
                delay(100); //evita di intasare la seriale
            }

            break;
        }

        case TAKEOFF: {        
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] TAKEOFF"));
            }
            
            long dt = elapsedTimeInState(); //temD){ //Se è apo trascorso nello stato
            int DDD = pSonar->getDistance(); //distanza rilevata dal sonar
            float temp = pTempSensor->getTemperature(); //temperatura rilevata dal sensore

            if (DDD <= D1 && (temp >= TEMP1 && dt > T3)){
                pendingPreAlarm = true; 
            } else if (DDD > D1 && dt > T1){
                pMotor->setPosition(HD_CLOSE); //Apre hangar
                pLcd->clear();
                pLcd->print("DRONE OUT");
                pContext->setDisplayState(DisplayState::DRONE_OUT);
                setState(DRONE_OUT);
            }

            break;       
        }

        case DRONE_OUT: {        
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] DRONE_OUT"));
            }

            //Se è rilevato il preallarme
            if(pendingPreAlarm){
                setState(PRE_ALARM);
            } 
            
            //Se è arrivato il comando di atterraggio e il drone è stato rilevato dal PIR
            else if((Serial.read() == DRU_OPENING) && pPir->isDetected()){ 

                pMotor->setPosition(HD_OPEN); //Apre hangar
                pLcd->clear();
                pLcd->print("LANDING");
                pContext->setDisplayState(DisplayState::LANDING);
                setState(LANDING);
            } else {
                //Ciclo su se stesso
                delay(100); //evita di intasare la seriale

            }
        }

        case LANDING: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] LANDING"));
            }

            long dt = elapsedTimeInState(); //temD){ //Se è apo trascorso nello stato
            int DDD = pSonar->getDistance(); //distanza rilevata dal sonar
            float temp = pTempSensor->getTemperature(); //temperatura rilevata dal sensore

            if (DDD >= D2 && (temp >= TEMP1 && dt > T3)){
                pendingPreAlarm = true; 
            } else if (DDD < D2 && dt > T2){
                pMotor->setPosition(HD_CLOSE); //Chiude hangar
                pLcd->clear();
                pLcd->print("DRONE INSIDE");
                pContext->setDisplayState(DisplayState::IDLE);
                setState(IDLE);
            }

            break;
        }

        case PRE_ALARM: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] PRE-ALARM"));
                
            }

            float temp = pTempSensor->getTemperature(); //temperatura rilevata dal sensore
            long dt = elapsedTimeInState(); //temD){ //Se è apo trascorso nello stato
            if (temp >= TEMP2 && dt > T4){
                pMotor->setPosition(HD_CLOSE); //Chiude hangar
                pContext->setDisplayState(DisplayState::ALARM);
                setState(ALARM);
            } else if (temp < TEMP1){
                pendingPreAlarm = false;
                pLcd->clear();
                pLcd->print("DRONE INSIDE");
                pContext->setDisplayState(DisplayState::IDLE);
                setState(IDLE);
            }

            break;
        }

        case ALARM: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] ALARM"));
                pLcd->clear();
                pLcd->print("ALARM");
                pContext->setDisplayState(DisplayState::ALARM);
            }

            if (pButton->isPressed()){
                pendingPreAlarm = false;
                pContext->setDisplayState(DisplayState::IDLE);
                setState(IDLE);
            }

            break;
        }
    }
}

void ControlHangarTask::setState(int s){
    state = s; //Perchè da errore ?
    stateTimestamp = millis();
    justEntered = true;
}

long ControlHangarTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool ControlHangarTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}
