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
private:
    DisplayState displayState;

};

#endif
