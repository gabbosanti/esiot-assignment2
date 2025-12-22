#include "Logger.h"
#include "MsgService.h"

LoggerService Logger;  

void LoggerService::log(const String& msg){
    if (msg != lastMsg) {     // stampa solo se diverso
         MsgService.sendMsg("lo:"+msg);
         lastMsg = msg;
    }
}



