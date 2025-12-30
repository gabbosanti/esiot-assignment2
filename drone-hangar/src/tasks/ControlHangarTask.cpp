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

ControlHangarTask::ControlHangarTask(Button *pButton, ServoMotor *pMotor, Sonar *pSonar, Pir *pPir, TempSensorTMP36 *pTempSensor, Lcd *pLcd, Context *pContext) : pButton(pButton), pMotor(pMotor), pSonar(pSonar), pPir(pPir), pTempSensor(pTempSensor), pLcd(pLcd), pContext(pContext)
{
    setState(IDLE);
    pLcd->init();
    pContext->setPendingPreAlarm(false);
}

void ControlHangarTask::tick()
{
    switch (state)
    {

    case IDLE:
    {
        readSerial(true);

        if (this->checkAndSetJustEntered())
        {
            Logger.log(F("[CHT] IDLE"));
            pLcd->print("DRONE INSIDE");
            pMotor->setPosition(HD_CLOSE);
            pContext->setDisplayState(DisplayState::DRONE_INSIDE);
            resetConditions();
        }

        unsigned int elapsedT1 = checkTemp(ID_TEMP1, TEMP1); // Controlla se la temperatura ha superato TEMP1 e ne ritorna il tempo
        Logger.log("Temperatura superiore a " + String(TEMP1) + " per " + String(elapsedT1) + " ms");
        if ((elapsedT1 > T3) || pContext->isPendingPreAlarm())
        {
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

        unsigned int elapsedT1 = checkTemp(ID_TEMP1, TEMP1); // Controlla se la temperatura ha superato TEMP1 e ne ritorna il tempo
        Logger.log("Temperatura superiore a " + String(elapsedT1) + " per " + elapsedT1 + " ms");

        unsigned int distanceD1 = checkDist(ID_DIST1, D1, '>'); // Controlla se la distanza ha superato D1 e ne ritorna il tempo
        Logger.log("Distanza superiore a " + String(D1) + " per " + distanceD1 + " ms");

        if (elapsedT1 > T3 && !pContext->isPendingPreAlarm())
        { // Altrimenti entra sempre qui (ciclo infinito)
            pContext->setPendingPreAlarm(true);
            Logger.log(F("[CHT] PENDING PRE-ALARM SET")); // Utilizzare per debug
        }
        else if (distanceD1 > T1)
        {
            pMotor->setPosition(HD_CLOSE); // Apre hangar
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
        readSerial(true);
        pPir->sync();

        if (this->checkAndSetJustEntered())
        {
            Logger.log(F("[CHT] DRONE_OUT"));
        }

        // Se nella fase di TAKEOFF è stata rilevata una condizione di pre-allarme (TAKEOFF già avvenuto)
        if (pContext->isPendingPreAlarm())
        {
            Logger.log(F("[CHT] MOVING TO PRE-ALARM")); // Utilizzare per debug
            setState(PRE_ALARM);
        }

        // Se è arrivato il comando di atterraggio e il drone è stato rilevato dal PIR
        if (pendingCmd == DRU_OPENING)
        {
            pendingCmd = "";
            Logger.log("LANDING command received");
            landingRequest = true;
        }

        // Se è stato richiesto l'atterraggio e il drone è stato rilevato dal PIR
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

        unsigned int elapsedT1 = checkTemp(ID_TEMP1, TEMP1); // Controlla se la temperatura ha superato TEMP1 e ne ritorna il tempo
        Logger.log("Temperatura superiore a " + String(elapsedT1) + " per " + elapsedT1 + " ms");

        unsigned int distanceD2 = checkDist(ID_DIST2, D2, '<'); // Controlla se la distanza è inferiore a D2 e ne ritorna il tempo
        Logger.log("Distanza inferiore a " + String(D2) + " per " + distanceD2 + " ms");

        if (elapsedT1 > T3)
        {
            pContext->setPendingPreAlarm(true);
            Logger.log(F("[CHT] PENDING PRE-ALARM SET")); // Utilizzare per debug
        }
        else if (distanceD2 > T2)
        {
            pMotor->setPosition(HD_CLOSE); // Chiude hangar
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

        float temp = pTempSensor->getTemperature();          // temperatura rilevata dal sensore
        unsigned int elapsedT4 = checkTemp(ID_TEMP2, TEMP2); // Controlla se la temperatura ha superato TEMP2 e ne ritorna il tempo

        if (temp >= TEMP2 && elapsedT4 > T4)
        {
            pMotor->setPosition(HD_CLOSE); // Chiude hangar
            pContext->setDisplayState(DisplayState::ALARM);
            setState(ALARM);
        }
        else if (temp < TEMP1)
        {
            if (droneOutside)
            {
                pLcd->clear();
                pLcd->print("DRONE_OUT");
                pContext->setDisplayState(DisplayState::DRONE_OUT);
                setState(DRONE_OUT);
            }
            else
            {
                pLcd->clear();
                pLcd->print("DRONE INSIDE");
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
            pContext->setDisplayState(DisplayState::ALARM);
        }

        if (pButton->isPressed())
        {
            pContext->setPendingPreAlarm(false);
            pContext->setDisplayState(DisplayState::DRONE_INSIDE);
            setState(IDLE);
        }

        break;
    }
    }
}

void ControlHangarTask::setState(State s)
{
    state = s; // Perchè da errore ?
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

    Logger.log("TEMPERATURA : " + String(temp));

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

    Logger.log("DISTANZA : " + String(d));

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

void ControlHangarTask::readSerial(bool allowed)
{
    if (!allowed)
        return;

    if (Serial.available() > 0)
    {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        if (cmd.length() > 0)
        {
            pendingCmd = cmd;
            Logger.log("Serial CMD buffered: " + cmd);
        }
    }
}