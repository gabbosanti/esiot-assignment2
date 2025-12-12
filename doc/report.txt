The system manages an automated hangar for a drone by controlling its exit and landing. Moreover it provides an alarm system that check if the temperature is rising for security reason. The system is modeled by two concurrent tasks (FSMs) that interact: ControlHangar Task and ControlButton Task.

The ControlHangar Task FSM is responsible for managing the physical status of the drone and the hangar door, and ensuring operational safety based on environmental conditions. 

--Normal Operational Cycle (Exit and Entry)---
The FSM begins in the IDLE state, where the drone is assumed to be inside the hangar, awaiting command.

--Drone Exit Sequence (Take-off)--
 It’s initiated when a request to activate the drone (dru-activate) is received and the hangar door is closed ( !hd-open ).
This triggers the transition from IDLE to TAKE-OFF state in which the door opening mechanism is activated ( hd-open ). TAKE-OFF state persist until the drone has successfully exit the hangar. Once a specific distance (ddd) has been traveled, and a designated time (T1) has passed, the system transitions to DRONE_OUT state and the door close (!hd-open).

--Drone Entry Sequence (Landing)—
 Begins upon receiving a java signal (dru-opening) indicating the drone’s return. If, upon receiving the command, the DPD detects the presence of the drone, the HD door opens and the LCD displays LANDING. The system then waits for the drone to enter and land. When the distance measured by the DDD is less than D2 for more than T2 seconds, it is assumed that the drone has landed, and the door is closed. The LCD then displays DRONE INSIDE.

--Pre-Alarm and Alarm—
It is driven by the ambient temperature (temp). If the temperature exceeds a threshold (temp> TEMP1) and persists for at least T3 seconds, the FSM transitions to PRE_ALARM state. Upon entering this state, the boolean flag, pendingPreAlarm, is set to true. While in PRE_ALARM, the system suspends new operational commands (take-offs and landings) but allows any currently active operation to safely reach completion.
If the temperature continues to rise, reaching a more critical level (temp > TEMP2) and persisting for another period (T4), the system immediately enters the ALARM state.  In this state all operations are suspended, the door is checked and commanded to close if it is not already (!hd-open). If the drone is out, the alarm message is sent through a java signal (DRU). The operations are re-enabled when the RESET button is pressed by an operator.

The ControlButton Task FSM is dedicated to managing the user interface and alarm signaling, primarily through the control panel's LED lights (L1, L2, L3) and reacting to status flags set by the ControlHangar Task.

---Idle State and Operational Signaling---
In the default state IDLE,  the display show the two possible state of the drone:  "DRONE INSIDE" or "DRONE OUT", only L1 is on.
When an active operation is initiated in the hangar ( "TAKE OFF" or "LANDING"), the FSM transitions from IDLE to the L2_ON/L2_OFF cycle, in which the led L2 start blinking. 
The blinking continues as long as the hangar task is active. Once the physical action is complete the system returns to the IDLE state.

--Alarm Sequence--
The FSM monitors the pendingPreAlarm boolean flag, which is set by the ControlHangar Task ( due to an high temperature detection).
It enters PRE_ALARM if the pendingPreAlarm flag becomes true, the system turns of the L2 blinking cycle (if on).
It enters LED_ALARM if the alarm condition persists and  is confirmed by the hangar task. In this critical state, LED L3 is activated and remains ON to provide a persistent and highly visible warning to the user.
The system return to IDLE when the pendingPreAlarm flag is off. This happens when the user releases the reset button.
