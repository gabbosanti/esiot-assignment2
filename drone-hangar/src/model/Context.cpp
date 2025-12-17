#include "Context.h"

Context::Context(){
    // inizializza lo stato di visualizzazione predefinito
    displayState = DisplayState::DRONE_INSIDE;
    pendingPreAlarm = false;
}

void Context::setDisplayState(DisplayState s){
    displayState = s;
}

DisplayState Context::getDisplayState(){
    return displayState;
}

void Context::setPendingPreAlarm(bool flag_prealarm) {
    pendingPreAlarm = flag_prealarm;
}

bool Context::isPendingPreAlarm(){
    return pendingPreAlarm;
}