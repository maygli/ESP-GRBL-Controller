#include "GrblCtrl_Board.h"
#include "GrblCtrl_Debug.h"

#include <LittleFS.h>

#define SD_CS_PIN 5

GrblCtrl_Board::GrblCtrl_Board()
{
    Serial.begin(115200);         // Start the Serial communication to send messages to the computer
    LittleFS.begin();
    m_CtrlCfg.setInternalFS(&LittleFS);
    m_CtrlCfg.setExternalFS(&SDFS);
    m_HttpServer.setConfig(&m_CtrlCfg);
    m_HttpServer.setFileSystem(&LittleFS);
}

void GrblCtrl_Board::init()
{
    m_SDMgr.init();
//    m_CtrlCfg.init();
    m_HttpServer.init();
}

void GrblCtrl_Board::process()
{
    m_HttpServer.process();
    m_SDMgr.process();
}
