#ifndef CONTROLLER_BOARD_H
#define CONTROLLER_BOARD_H

#include "GrblCtrl_Config.h"
#include "GrblCtrl_Http.h"
#include "GrblCtrl_SDManager.h"

#include <Arduino.h>
#include <FS.h>   // Include the SPIFFS library

class GrblCtrl_Board{
public:    
    GrblCtrl_Board();
    void init();
    void process();
protected:
    GrblCtrl_Config     m_CtrlCfg;
    GrblCtrl_Http       m_HttpServer;
    GrblCtrl_SDManager  m_SDMgr;
    static const String m_sIntConfigFileName;
    static const String m_sExtConfigFileName;
};

#endif