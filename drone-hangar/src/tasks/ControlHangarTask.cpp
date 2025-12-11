#include "ControlHangarTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

#define FWD_TIME 2000
#define BWD_TIME 1000
#define START_TIME 100
#define RESET_TIME 500
#define DRU_ACTIVATE 'OPEN DOOR'

ControlHangarTask::ControlHangarTask(Button* pButton, ServoMotor* pMotor, Sonar* pSonar, Pir * pPir, TempSensorTMP36* pTempSensor, Lcd* pLcd, Context* pContext): 
    pMotor(pMotor), pButton(pButton), pTempSensor(pTempSensor), pLcd(pLcd), pContext(pContext){
    setState(IDLE);
}
  
void ControlHangarTask::tick(){
    switch (state){    
    case IDLE: {
        if (this->checkAndSetJustEntered()){
            Logger.log(F("[CHT] IDLE"));
            pContext->setDisplayState(DisplayState::IDLE);
        }
        if (Serial.read() == DRU_ACTIVATE){ //Se è arrivato il comando di apertura hangar
            
            pMotor->on();
            pLcd->print("TAKEOFF");       
            pContext->setDisplayState(DisplayState::TAKEOFF);
            setState(TAKEOFF);
        }
        else{
            delay(100); //evita di intasare la seriale
            
        }


        break;
    }
    case LANDING: {        
        if (this->checkAndSetJustEntered()){
            Logger.log(F("[SWT] LANDING"));
        }
        
        /* update motor pos*/

        long dt = elapsedTimeInState();
        currentPos = (((float) dt)/FWD_TIME)*180;
        pMotor->setPosition(currentPos);

        if (pButton->isPressed()){
            pContext->setStopped();
            Logger.log(F("[SWT] STOPPED!"));
            setState(RESETTING);
        } else if (dt > FWD_TIME){
            setState(SWEEPING_BWD);
        }
        break;       
    }
    case SWEEPING_BWD: {        
        if (this->checkAndSetJustEntered()){
            Logger.log(F("[SWT] SWEEPING_BWD"));
        }

        /* update motor pos*/
        
        long dt = elapsedTimeInState();
        currentPos = (((float) dt)/BWD_TIME)*180;
        pMotor->setPosition(currentPos);

        if (pButton->isPressed()){
            Logger.log(F("[SWT] STOPPED!"));
            pContext->setStopped();
            toBeStopped = true;
        } 
        
        if (dt > BWD_TIME){
            if (!toBeStopped){
                setState(SWEEPING_FWD);
            } else {
                setState(RESETTING);    
            }
        }
        break;       
    }
    /*
    case STARTING: {
        if (this->checkAndSetJustEntered()){
            Logger.log(F("[SWT] STARTING"));
        }
        if (elapsedTimeInState() > START_TIME){
            setState(SWEEPING_FWD);
        }
        break;
    }*/
    case RESETTING: {
        if (this->checkAndSetJustEntered()){
            Logger.log(F("[SWT] RESETTING"));
        }
        pMotor->setPosition(0);
        if (elapsedTimeInState() > RESET_TIME){
            pMotor->off();
            setState(IDLE);
        }
    }
    }
}

void ControlHangarTask::setState(int s){
    state = s;
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
