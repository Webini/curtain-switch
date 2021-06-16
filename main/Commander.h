#ifndef COMMANDER_H
#define COMMANDER_H

#include "GlobalConfiguration.h"

class Commander {
  public:
    Commander(GlobalConfiguration* globalConf);
    void begin();
    void loop();
  
  private:
    GlobalConfiguration* conf; 
};

#endif
