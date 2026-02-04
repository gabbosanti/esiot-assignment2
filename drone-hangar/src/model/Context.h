#ifndef __CONTEXT__
#define __CONTEXT__
#include "DisplayState.h"

/* Sharing Global state between Tasks
*/
class Context {

public:
    Context();
    void setDisplayState(DisplayState s);
    DisplayState getDisplayState();
    
    void setPendingPreAlarm(bool flag_prealarm);
    bool isPendingPreAlarm();

private:
    DisplayState displayState;
    bool pendingPreAlarm;

};

#endif
