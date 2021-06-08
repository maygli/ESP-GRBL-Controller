#include "GrblCtrl_Board.h"

#include <LittleFS.h>
#include <SD.h>
#include <SDFS.h>

#define SD_CS_PIN 5

GrblCtrl_Board::GrblCtrl_Board()
{
    Serial.begin(115200);         // Start the Serial communication to send messages to the computer
    LittleFS.begin();
    SD.begin(SD_CS_PIN);
    m_CtrlCfg.setInternalFS(&LittleFS);
    m_CtrlCfg.setExternalFS(&SDFS);
    m_HttpServer.setConfig(&m_CtrlCfg);
    m_HttpServer.setFileSystem(&LittleFS);
}

void GrblCtrl_Board::init()
{
    m_CtrlCfg.init();
    m_HttpServer.init();
}

void GrblCtrl_Board::process()
{
    m_HttpServer.process();
}
