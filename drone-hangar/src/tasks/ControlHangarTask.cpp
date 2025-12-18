#include "ControlHangarTask.h"
#include "Arduino.h"
#include "config.h"
#include "kernel/Logger.h"

#define DRU_ACTIVATE "ACTIVATE"
#define DRU_OPENING "OPEN"
#define ID_TEMP1 1
#define ID_TEMP2 2
#define ID_DIST1 1
#define ID_DIST2 2

ControlHangarTask::ControlHangarTask(Button* pButton, ServoMotor* pMotor, Sonar* pSonar, Pir * pPir, TempSensorMock* pTempSensor, Lcd* pLcd, Context* pContext): 
   pButton(pButton), pMotor(pMotor), pSonar(pSonar), pPir(pPir), pTempSensor(pTempSensor), pLcd(pLcd), pContext(pContext){
    setState(IDLE);
    pLcd->init();
    pContext->setPendingPreAlarm(false);
}
  
void ControlHangarTask::tick(){
    switch (state){    

        case IDLE: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] IDLE"));
                pLcd->print("DRONE INSIDE");
                pMotor->setPosition(HD_CLOSE);
                pContext->setDisplayState(DisplayState::DRONE_INSIDE);
            }

            unsigned int elapsedT1 = checkTemp(ID_TEMP1, TEMP1); //Controlla se la temperatura ha superato TEMP1 e ne ritorna il tempo
            if((elapsedT1 > T3) || pContext->isPendingPreAlarm()){
                setState(PRE_ALARM);
            }

            else if  (Serial.available() > 0) {
                String cmd = Serial.readStringUntil('\n');  // legge una riga
                cmd.trim();                                 // toglie spazi/\r

                Logger.log("Messaggio ricevuto sulla serial LINE " + cmd);
                if (cmd == DRU_ACTIVATE) {
                    Logger.log("Messaggio corretto");
                    pMotor->on();
                    pMotor->setPosition(HD_OPEN);
                    pLcd->clear();
                    pLcd->print("TAKEOFF");
                    pContext->setDisplayState(DisplayState::TAKEOFF);
                    setState(TAKEOFF);
                }
            }

            break;
        }

        case TAKEOFF: {        
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] TAKEOFF"));
            }
            
            unsigned int elapsedT1 = checkTemp(ID_TEMP1, TEMP1); //Controlla se la temperatura ha superato TEMP1 e ne ritorna il tempo
            unsigned int distanceD1 = checkDist(ID_DIST1, D1, '>'); //Controlla se la distanza ha superato D1 e ne ritorna il tempo
            Logger.log("DISTANZA : " + String(pSonar->getDistance()));
            
            if (elapsedT1 > T3 && !pContext->isPendingPreAlarm()){ //Altrimenti entra sempre qui (ciclo infinito)
                pContext->setPendingPreAlarm(true);
                Logger.log(F("[CHT] PENDING PRE-ALARM SET")); //Utilizzare per debug
            } else if (distanceD1 > T1){ 
                pMotor->setPosition(HD_CLOSE); //Apre hangar
                droneOutside = true;
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

            //Se nella fase di TAKEOFF è stata rilevata una condizione di pre-allarme (TAKEOFF già avvenuto)
            if(pContext->isPendingPreAlarm()){
                Logger.log(F("[CHT] MOVING TO PRE-ALARM")); //Utilizzare per debug
                setState(PRE_ALARM);
            } 
            
            //Se è arrivato il comando di atterraggio e il drone è stato rilevato dal PIR
            else if(pPir->isDetected()){ 
                Logger.log("PIR ha rilevato un movimento");
                if (Serial.available() > 0) {
                    String cmd = Serial.readStringUntil('\n');
                    cmd.trim();

                    Logger.log("Messaggio ricevuto sulla serial LINE " + cmd);

                    if (cmd == DRU_OPENING) {
                        Logger.log("Messaggio corretto");
                        pMotor->setPosition(HD_OPEN); //Apre hangar
                        pLcd->clear();
                        pLcd->print("LANDING");
                        pContext->setDisplayState(DisplayState::LANDING);
                        setState(LANDING);
                    }
                }
            }

            break;
        }

        case LANDING: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] LANDING"));
            }

            int DDD = pSonar->getDistance(); //distanza rilevata dal sonar
            unsigned int elapsedT1 = checkTemp(ID_TEMP1, TEMP1); //Controlla se la temperatura ha superato TEMP1 e ne ritorna il tempo
            unsigned int distanceD2 = checkDist(ID_DIST2, D2, '<'); //Controlla se la distanza è inferiore a D2 e ne ritorna il tempo

            if (elapsedT1 > T3){
                pContext->setPendingPreAlarm(true); 
                Logger.log(F("[CHT] PENDING PRE-ALARM SET")); //Utilizzare per debug
            } else if (DDD < D2 && distanceD2 > T2){
                pMotor->setPosition(HD_CLOSE); //Chiude hangar
                pLcd->clear();
                pLcd->print("DRONE INSIDE");
                pContext->setDisplayState(DisplayState::DRONE_INSIDE);
                setState(IDLE);
            }

            break;
        }

        case PRE_ALARM: {
            if (this->checkAndSetJustEntered()){
                Logger.log(F("[CHT] PRE-ALARM"));
                
            }
            
            float temp = pTempSensor->getTemperature(); //temperatura rilevata dal sensore
            unsigned int elapsedT4 = checkTemp(ID_TEMP2, TEMP2); //Controlla se la temperatura ha superato TEMP2 e ne ritorna il tempo
            
            if (temp >= TEMP2 && elapsedT4 > T4){
                pMotor->setPosition(HD_CLOSE); //Chiude hangar
                pContext->setDisplayState(DisplayState::ALARM);
                setState(ALARM);
            } else if (temp < TEMP1){
                if(droneOutside){
                    pLcd->clear();
                    pLcd->print("DRONE_OUT");
                    pContext->setDisplayState(DisplayState::DRONE_OUT);
                    setState(IDLE);
                } else{
                    pLcd->clear();
                    pLcd->print("DRONE INSIDE");
                    pContext->setDisplayState(DisplayState::DRONE_INSIDE);
                    setState(IDLE);
                }
                pContext->setPendingPreAlarm(false);
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
                pContext->setPendingPreAlarm(false);
                pContext->setDisplayState(DisplayState::DRONE_INSIDE);
                setState(IDLE);
            }

            break;
        }
    }
}

void ControlHangarTask::setState(State s){
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

// Controlla se la temperatura ha superato una certa soglia (TEMP1 o TEMP2) e per quanto tempo consecutivamente
unsigned int ControlHangarTask::checkTemp(unsigned int id, float soglia) {
    
    float temp = pTempSensor->getTemperature(); //temperatura rilevata dal sensore

    bool  &flag   = (id == ID_TEMP1) ? temp1Cond : temp2Cond; // Seleziona il flag corretto in base all'id
    unsigned int  &start  = (id == ID_TEMP1) ? temp1Start : temp2Start; // Seleziona il tempo di inizio corretto in base all'id

    if (temp >= soglia) { 
        if (!flag) { //Se la condizione era falsa (es. prima volta che viene soddisfatta o è stata resettata)
            flag = true; 
            start = millis(); 
        }
        return millis() - start;
    } else {
        flag = false;
        return 0;
    }
}

// Controlla se la distanza ha superato una certa soglia (D1 o D2) e per quanto tempo consecutivamente
unsigned int ControlHangarTask::checkDist(unsigned int id, unsigned int soglia, char op){

    unsigned  d = pSonar->getDistance();

    bool  &flag   = (id == ID_DIST1) ? d1Cond   : d2Cond;
    auto  &start  = (id == ID_DIST1) ? d1Start  : d2Start;

    bool cond = (op == '>') ? (d > soglia)
              : (op == '<') ? (d < soglia)
                            : false;

    if (cond) {
        if (!flag) { flag = true; start = millis(); }
        return millis() - start;
    } else {
        flag = false;
        return 0;
    }
}