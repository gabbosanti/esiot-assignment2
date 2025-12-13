#include "ControlHangarTask.h"
#include "Arduino.h"
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
                pMotor->setPosition(HD_CLOSE);
                pContext->setDisplayState(DisplayState::IDLE);
            }

            unsigned int elapsedT1 = checkTemp(TEMP1); //Controlla se la temperatura ha superato TEMP1 e ne ritorna il tempo

            if((elapsedT1 > T3) || pendingPreAlarm){
                setState(PRE_ALARM);
            }

            else if (Serial.read() == DRU_ACTIVATE){ //Se è arrivato il comando di apertura hangar
                
                pMotor->on(); //Attiva servo
                pMotor->setPosition(HD_OPEN); //Apre l'hangar
                pLcd->clear();
                pLcd->print("TAKEOFF");       
                pContext->setDisplayState(DisplayState::TAKEOFF);
                setState(TAKEOFF);
            }

            break;
        }

        case TAKEOFF: {        
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] TAKEOFF"));
            }
            
            int DDD = pSonar->getDistance(); //distanza rilevata dal sonar
            unsigned int elapsedT1 = checkTemp(TEMP1); //Controlla se la temperatura ha superato TEMP1 e ne ritorna il tempo
            unsigned int distanceD1 = checkDist(D1, '>'); //Controlla se la distanza ha superato D1 e ne ritorna il tempo
            
            if (DDD <= D1 && elapsedT1 > T3){
                pendingPreAlarm = true; 
                Logger.log(F("[CHT] PENDING PRE-ALARM SET")); //Utilizzare per debug
            } else if (DDD > D1 && distanceD1 > T1){ 
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
                Logger.log(F("[CHT] MOVING TO PRE-ALARM")); //Utilizzare per debug
                setState(PRE_ALARM);
            } 
            
            //Se è arrivato il comando di atterraggio e il drone è stato rilevato dal PIR
            else if(pPir->isDetected() && (Serial.read() == DRU_OPENING) ){ 

                pMotor->setPosition(HD_OPEN); //Apre hangar
                pLcd->clear();
                pLcd->print("LANDING");
                pContext->setDisplayState(DisplayState::LANDING);
                setState(LANDING);
            }

            break;
        }

        case LANDING: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] LANDING"));
            }

            int DDD = pSonar->getDistance(); //distanza rilevata dal sonar
            float temp = pTempSensor->getTemperature(); //temperatura rilevata dal sensore
            unsigned int elapsedT1 = checkTemp(TEMP1); //Controlla se la temperatura ha superato TEMP1 e ne ritorna il tempo
            unsigned int distanceD2 = checkDist(D2, '<'); //Controlla se la distanza è inferiore a D2 e ne ritorna il tempo

            if (DDD >= D2 && elapsedT1 > T3){
                pendingPreAlarm = true; 
                Logger.log(F("[CHT] PENDING PRE-ALARM SET")); //Utilizzare per debug
            } else if (DDD < D2 && distanceD2 > T2){
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
            unsigned int elapsedT4 = checkTemp(TEMP2); //Controlla se la temperatura ha superato TEMP2 e ne ritorna il tempo

            if (temp >= TEMP2 && elapsedT4 > T4){
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

// Controlla se la temperatura ha superato una certa soglia (TEMP1 o TEMP2)
unsigned int ControlHangarTask::checkTemp(float TEMP){

    float temp = pTempSensor->getTemperature(); //temperatura rilevata dal sensore
    bool tempHigh = (temp >= TEMP); //flag temperatura alta
    unsigned int tempHighStart; //tempo in cui la temperatura è diventata alta
    unsigned int tempHighElapsed; //tempo trascorso con temperatura alta

    if (temp >= TEMP) {
        if (!tempHigh) {            // appena superata la soglia
            tempHigh = true;
            tempHighStart = millis();
    }    
    
    tempHighElapsed = millis() - tempHighStart;
    } else {
        tempHigh = false;
        tempHighElapsed = 0;
    }

    return tempHighElapsed;
}

unsigned int ControlHangarTask::checkDist(int DIST, char OPERATOR){

    int DDD = pSonar->getDistance();
    
    bool flagDist =
        (OPERATOR == '>') ? (DDD > DIST) : //Se l'operatore è '>' controlla se la distanza è maggiore di DIST
        (OPERATOR == '<') ? (DDD < DIST) : //Se l'operatore è '<' controlla se la distanza è minore di DIST
        false;    // default

    if (flagDist) { //condizione distanza verificata
        if (!distCond) {
            distCond = true;
            distCondStart = millis();
        }
        distCondElapsed = millis() - distCondStart;
    } else {
        distCond = false;
        distCondElapsed = 0;
    }
    return distCondElapsed;
}