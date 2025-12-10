#include "tasks/BlinkingTask.h"
#include <Arduino.h>
#include "config.h"
#include "kernel/Logger.h"

BlinkingTask::BlinkingTask(Led* pLed1, Led* pLed2, Led* pLed3, Context* pContext): 
    pContext(pContext), pLed1(pLed1), pLed2(pLed2), pLed3(pLed3){
    setState(IDLE);
}
  
void BlinkingTask::tick(){

    bool inBlinkingPhase = pContext->isStarted();

    if(!inBlinkingPhase && state != IDLE){
        setState(IDLE);
    }

    switch (state){   
    case IDLE: {
        if (this->checkAndSetJustEntered()){
            pLed1->switchOn();
            pLed2->switchOff();
            pLed3->switchOff();
            Logger.log(F("[BT] L1 ON, L2 OFF, L3 OFF"));

        }
        if (inBlinkingPhase()){
            setState(BLINK_OFF);
        }
        break;
    }
    case L2_ON: {
        if (this->checkAndSetJustEntered()){
            pLed->switchOff();
            Logger.log(F("[BT] BLINK_OFF"));
        }
        if (elapsedTimeInState() >= (L2_BLINK / 2)){
            setState(BLINK_ON);
        }  
        if(!inBlinkingPhase){
            setState(IDLE);
        }
        break;
    }
    case L2_OFF: {
        if (this->checkAndSetJustEntered()){
            pLed->switchOn();
            Logger.log(F("[BT] BLINK_ON"));
        }
        if (elapsedTimeInState() >= (L2_BLINK / 2)){
            setState(BLINK_OFF);
        }  
        if(!inBlinkingPhase){
            setState(IDLE);
        }
        break;
    }
    case PRE_ALARM: {
        if (this->checkAndSetJustEntered()){
            pLed->switchOn();
            Logger.log(F("[BT] BLINK_ON"));
        }
        if (elapsedTimeInState() >= (L2_BLINK / 2)){
            setState(BLINK_OFF);
        }  
        if(!inBlinkingPhase){
            setState(IDLE);
        }
        break;
    }
    case LED_ALARM: {
        if (this->checkAndSetJustEntered()){
            pLed->switchOn();
            Logger.log(F("[BT] BLINK_ON"));
        }
        if (elapsedTimeInState() >= (L2_BLINK / 2)){
            setState(BLINK_OFF);
        }  
        if(!inBlinkingPhase){
            setState(IDLE);
        }
        break;
    }
    default:
        setState(IDLE);
        break;
    
    }
}


void BlinkingTask::setState(int s){
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long BlinkingTask::elapsedTimeInState(){
    return millis() - stateTimestamp;
}

bool BlinkingTask::checkAndSetJustEntered(){
    bool bak = justEntered;
    if (justEntered){
      justEntered = false;
    }
    return bak;
}
