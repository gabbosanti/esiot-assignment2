#include "tasks/BlinkingTask.h"
#include "Arduino.h"
#include "config.h"
#include "kernel/Logger.h"

BlinkingTask::BlinkingTask(Led *pLed1, Led *pLed2, Led *pLed3, Context *pContext) : pLed1(pLed1), pLed2(pLed2), pLed3(pLed3), pContext(pContext)
{
    setState(IDLE);
}

void BlinkingTask::tick()
{

    DisplayState display = pContext->getDisplayState();

    switch (state)
    {
    case IDLE:
    {

        if (this->checkAndSetJustEntered())
        {
            pLed1->switchOn();
            pLed2->switchOff();
            pLed3->switchOff();
            Logger.log(F("[BT] L1 ON, L2 OFF, L3 OFF"));
        }

        if (display == DisplayState::TAKEOFF || display == DisplayState::LANDING)
        {
            setState(L2_ON);
        }

        break;
    }
    case L2_ON:
    {

        if (this->checkAndSetJustEntered())
        {
            pLed2->switchOn();
            Logger.log(F("[BT] L1 ON L2 ON L3 OFF"));
        }

        // Se ho superato il periodo
        if (elapsedTimeInState() >= (L2_BLINK))
        {
            setState(L2_OFF);
        }

        if (pContext->isPendingPreAlarm())
        {
            pLed2->switchOff();
            setState(PRE_ALARM);
        }
        else if (display == DisplayState::DRONE_OUT || display == DisplayState::DRONE_INSIDE)
        {
            pLed2->switchOff();
            setState(IDLE);
        }

        break;
    }
    case L2_OFF:
    {
        if (this->checkAndSetJustEntered())
        {
            pLed2->switchOff();
            Logger.log(F("[BT] L1 ON L2 OFF L3 OFF"));
        }

        if (elapsedTimeInState() >= (L2_BLINK))
        {
            setState(L2_ON);
        }

        if (pContext->isPendingPreAlarm())
        {
            setState(PRE_ALARM);
        }
        else if (display == DisplayState::DRONE_OUT || display == DisplayState::DRONE_INSIDE)
        {
            setState(IDLE);
        }

        break;
    }
    case PRE_ALARM:
    {
        if (this->checkAndSetJustEntered())
        {
            Logger.log(F("[BT] PRE_ALARM"));
        }
        if (pContext->getDisplayState() == DisplayState::ALARM)
        {
            setState(LED_ALARM);
        }

        break;
    }
    case LED_ALARM:
    {
        if (this->checkAndSetJustEntered())
        {
            pLed3->switchOn();
            Logger.log(F("[BT] LED_ALARM"));
        }
        if (pContext->getDisplayState() == DisplayState::DRONE_INSIDE)
        {
            pLed3->switchOff();
            setState(IDLE);
        }
        break;
    }
    default:
        setState(IDLE);
        break;
    }
}

void BlinkingTask::setState(State s)
{
    state = s; // Perchè da errore ?
    stateTimestamp = millis();
    justEntered = true;
}

long BlinkingTask::elapsedTimeInState()
{
    return millis() - stateTimestamp;
}

bool BlinkingTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}
