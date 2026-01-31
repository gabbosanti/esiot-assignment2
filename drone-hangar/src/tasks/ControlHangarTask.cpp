#include "ControlHangarTask.h"
#include "Arduino.h"
#include "config.h"
#include "kernel/Logger.h"
#include "kernel/MsgService.h"

#define DRU_ACTIVATE "ACTIVATE"
#define DRU_OPENING "OPEN"
#define ID_TEMP1 1
#define ID_TEMP2 2
#define ID_DIST1 1
#define ID_DIST2 2

ControlHangarTask::ControlHangarTask(Button *pButton, ServoMotor *pMotor, Sonar *pSonar, Pir *pPir, TempSensorTMP36 *pTempSensor, Lcd *pLcd, Context *pContext) : pButton(pButton), pMotor(pMotor), pSonar(pSonar), pPir(pPir), pTempSensor(pTempSensor), pLcd(pLcd), pContext(pContext)
{
    setState(IDLE);
    pLcd->init();
    pContext->setPendingPreAlarm(false);
}

void ControlHangarTask::tick()
{
    if (MsgService.isMsgAvailable())
    {
        Msg *msg = MsgService.receiveMsg();
        pendingCmd = msg->getContent();
        delete msg;
    }

    switch (state)
    {

    case IDLE:
    {

        if (this->checkAndSetJustEntered())
        {
            Logger.log(F("[CHT] IDLE"));
            pLcd->clear();
            pLcd->print("DRONE INSIDE");
            pMotor->setPosition(HD_CLOSE); // Closes hangar, if open
            pContext->setDisplayState(DisplayState::DRONE_INSIDE);
            resetConditions();
        }

        unsigned int elapsedT1 = checkTemp(ID_TEMP1, TEMP1); // Check if temperature exceeded TEMP1 and for how long
        Logger.log("Temperature higer than " + String(TEMP1) + " for " + String(elapsedT1) + " ms");
        if ((elapsedT1 > T3) || pContext->isPendingPreAlarm())
        {
            Logger.log(F("[CHT] MOVING TO PRE-ALARM"));
            pContext->setDisplayState(DisplayState::PREALARM);
            setState(PRE_ALARM);
        }

        if (pendingCmd == DRU_ACTIVATE)
        {
            pendingCmd = "";

            Logger.log("ACTIVATE command received");
            pMotor->on();
            pMotor->setPosition(HD_OPEN);
            pLcd->clear();
            pLcd->print("TAKEOFF");
            pContext->setDisplayState(DisplayState::TAKEOFF);
            setState(TAKEOFF);
        }

        break;
    }

    case TAKEOFF:
    {
        if (this->checkAndSetJustEntered())
        {
            Logger.log(F("[CHT] TAKEOFF"));
            resetConditions();
        }

        unsigned int elapsedT1 = checkTemp(ID_TEMP1, TEMP1); // Check if temperature exceeded TEMP1 and for how long
        Logger.log("Temperature higher than " + String(TEMP1) + " for " + String(elapsedT1) + " ms");

        unsigned int distanceD1 = checkDist(ID_DIST1, D1, '>'); // Check if distance exceeded D1 and for how long
        Logger.log("Distance higher than " + String(D1) + " for " + distanceD1 + " ms");

        if (elapsedT1 > T3 && !pContext->isPendingPreAlarm()) // If pending pre-alarm is already set, do not execute again
        {
            pContext->setPendingPreAlarm(true);
            Logger.log(F("[CHT] PENDING PRE-ALARM SET"));
        }

        else if (distanceD1 > T1)
        {
            pMotor->setPosition(HD_CLOSE); // opens hangar
            droneOutside = true;
            pLcd->clear();
            pLcd->print("DRONE OUT");
            pContext->setDisplayState(DisplayState::DRONE_OUT);
            setState(DRONE_OUT);
        }

        break;
    }

    case DRONE_OUT:
    {
        pPir->sync();

        if (this->checkAndSetJustEntered())
        {
            Logger.log(F("[CHT] DRONE_OUT"));
        }

        // If pre-alarm was pending, move to PRE-ALARM state
        if (pContext->isPendingPreAlarm())
        {
            Logger.log(F("[CHT] MOVING TO PRE-ALARM"));
            setState(PRE_ALARM);
        }

        // If LANDING command received via serial
        if (pendingCmd == DRU_OPENING)
        {
            pendingCmd = "";
            Logger.log("LANDING command received");
            landingRequest = true;
        }

        if (landingRequest && pPir->isDetected())
        {
            Logger.log("PIR detected + landing requested");
            landingRequest = false;
            pMotor->setPosition(HD_OPEN);
            pLcd->clear();
            pLcd->print("LANDING");
            pContext->setDisplayState(DisplayState::LANDING);
            setState(LANDING);
        }

        break;
    }

    case LANDING:
    {
        if (this->checkAndSetJustEntered())
        {
            Logger.log(F("[CHT] LANDING"));
            resetConditions();
        }

        unsigned int elapsedT1 = checkTemp(ID_TEMP1, TEMP1);
        Logger.log("Temperature higher than " + String(TEMP1) + " for " + String(elapsedT1) + " ms");

        unsigned int distanceD2 = checkDist(ID_DIST2, D2, '<'); // Check if distance is lower than D2 and for how long
        Logger.log("Distance lower than " + String(D2) + " for " + String(distanceD2) + " ms");

        if (elapsedT1 > T3)
        {
            pContext->setPendingPreAlarm(true);
            Logger.log(F("[CHT] PENDING PRE-ALARM SET"));
        }

        if (distanceD2 > T2)
        {
            pMotor->setPosition(HD_CLOSE); // Closes hangar
            droneOutside = false;
            pLcd->clear();
            pContext->setDisplayState(DisplayState::DRONE_INSIDE);
            setState(IDLE);
        }

        break;
    }

    case PRE_ALARM:
    {
        if (this->checkAndSetJustEntered())
        {
            Logger.log(F("[CHT] PRE-ALARM"));
        }

        float temp = pTempSensor->getTemperature();
        unsigned int elapsedT4 = checkTemp(ID_TEMP2, TEMP2); // Controlla se la temperatura ha superato TEMP2 e ne ritorna il tempo
        Logger.log("Temperature higher than " + String(TEMP2) + " for " + String(elapsedT4) + " ms");

        if (elapsedT4 > T4)
        {
            pMotor->setPosition(HD_CLOSE);
            pContext->setDisplayState(DisplayState::ALARM);
            setState(ALARM);
        }
        else if (temp < TEMP1) // If the temperature drops below Temp1, the system returns to normal operation
        {
            if (droneOutside)
            {
                pContext->setDisplayState(DisplayState::DRONE_OUT);
                setState(DRONE_OUT);
            }
            else
            {
                pContext->setDisplayState(DisplayState::DRONE_INSIDE);
                setState(IDLE);
            }

            pContext->setPendingPreAlarm(false);
        }

        break;
    }

    case ALARM:
    {
        if (this->checkAndSetJustEntered())
        {
            Logger.log(F("[CHT] ALARM"));
            pLcd->clear();
            pLcd->print("ALARM");
            pMotor->setPosition(HD_CLOSE);
            lastButtonState = false; // Reset quando entri
        }

        bool currentPressed = pButton->isPressed();

        if (currentPressed && !lastButtonState)
        {
            Logger.log("RESET button pressed");
            pContext->setPendingPreAlarm(false);
            resetConditions();
            pContext->setDisplayState(DisplayState::DRONE_INSIDE);
            setState(IDLE);
        }

        lastButtonState = currentPressed;
        break;
    }
    }
}

void ControlHangarTask::setState(State s)
{
    state = s;
    stateTimestamp = millis();
    justEntered = true;
}

long ControlHangarTask::elapsedTimeInState()
{
    return millis() - stateTimestamp;
}

bool ControlHangarTask::checkAndSetJustEntered()
{
    bool bak = justEntered;
    if (justEntered)
    {
        justEntered = false;
    }
    return bak;
}

// Controlla se la temperatura ha superato una certa soglia (TEMP1 o TEMP2) e per quanto tempo consecutivamente
unsigned int ControlHangarTask::checkTemp(unsigned int id, float soglia)
{

    float temp = pTempSensor->getTemperature(); // temperatura rilevata dal sensore

    Logger.log("TEMP : " + String(temp));

    bool &flag = (id == ID_TEMP1) ? temp1Cond : temp2Cond;            // Seleziona il flag corretto in base all'id
    unsigned int &start = (id == ID_TEMP1) ? temp1Start : temp2Start; // Seleziona il tempo di inizio corretto in base all'id

    if (temp >= soglia)
    {
        if (!flag)
        { // Se la condizione era falsa (es. prima volta che viene soddisfatta o è stata resettata)
            flag = true;
            start = millis();
        }
        return millis() - start;
    }
    else
    {
        flag = false;
        return 0;
    }
}

// Controlla se la distanza ha superato una certa soglia (D1 o D2) e per quanto tempo consecutivamente
unsigned int ControlHangarTask::checkDist(unsigned int id, float soglia, char op)
{

    float d = pSonar->getDistance();

    Logger.log("DISTANCE : " + String(d));

    bool &flag = (id == ID_DIST1) ? d1Cond : d2Cond;
    auto &start = (id == ID_DIST1) ? d1Start : d2Start;

    bool cond = (op == '>')   ? (d > soglia)
                : (op == '<') ? (d < soglia)
                              : false;

    if (cond)
    {
        if (!flag)
        {
            flag = true;
            start = millis();
        }
        return millis() - start;
    }
    else
    {
        flag = false;
        return 0;
    }
}

void ControlHangarTask::resetConditions()
{
    temp1Cond = false;
    temp2Cond = false;
    d1Cond = false;
    d2Cond = false;
    temp1Start = 0;
    temp2Start = 0;
    d1Start = 0;
    d2Start = 0;
}
