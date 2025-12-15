#include "Context.h"

Context::Context(){
    // inizializza lo stato di visualizzazione predefinito
    displayState = DisplayState::DRONE_INSIDE;
}

void Context::setDisplayState(DisplayState s){
    displayState = s;
}

DisplayState Context::getDisplayState(){
    return displayState;
}

