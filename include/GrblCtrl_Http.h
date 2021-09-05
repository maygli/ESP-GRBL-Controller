#ifndef GRBL_CTRL_HTTP_H
#define GRBL_CTRL_HTTP_H

#include "GrblCtrl_Config.h"
#include "GrblCtrl_SDManager.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <FS.h>

class GrblCtrl_Http{
public:
    GrblCtrl_Http();
    GrblCtrl_Http(GrblCtrl_Config* theConfig, FS* theFS);
    void init();
    void process();
    void setConfig(GrblCtrl_Config* theConfig);
    void setSDManager(GrblCtrl_SDManager* theManager);
    void setFileSystem(FS* theFS);
protected:
    void onBoardInfo();
    void onNotFound();
    bool handleFileRead(String thePath);
    String getContentType(String theFilename);
    void onUploadFiremwareParameters();
    void onUploadFirmwareFinished();
    void onUploadFirmware();
    void onSystemInfo();
protected:
    bool                m_isClear;
    FS*                 m_HtmlFS;
    GrblCtrl_Config*    m_Config;
    GrblCtrl_SDManager* m_SDManager;
    ESP8266WiFiMulti    m_WifiMulti;
    ESP8266WebServer    m_Server;
    File                m_UploadedFile;
    static const String m_sDefaultPage;
};

#endif