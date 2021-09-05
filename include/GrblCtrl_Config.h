
#ifndef GRBL_CTRL_CONFIG_H
#define GRBL_CTRL_CONFIG_H

#define ARDUINOJSON_USE_LONG_LONG  1

#include <LittleFS.h>
#include <ArduinoJson.h>

//Debug
#define DEBUG_UART0 

//Default parameter
//Initial mode Access Point - true, Station - false
#define CONFIG_INITIAL_AP_MODE false
//Default Access point name
#define CONFIG_AP_NAME "ESP-GRBL"
//Default Access point password. Should be at least 8 symbols, otherwise AP creation will failed
#define CONFIG_AP_PASSWORD "ESPGRBLESPGRBL"
//Stored SSID count. If this value is cahnged, it may need to change fillDefaults 
#define CONFIG_STORED_SSID_COUNT 4
//WiFi SSID1 name in station mode
#define CONFIG_SSID1 "MaygliHome"
//WiFi SSID2 name in station mode
#define CONFIG_SSID2 "ssid2"
//WiFi ssid 3 name in station mode
#define CONFIG_SSID3 "ssid3"
//WiFi ssid 4 name in station mode
#define CONFIG_SSID4 "ssid4"
//Password for AP with SSID1
#define CONFIG_PASSWORD1 "firebird"
//Password for AP with SSID2
#define CONFIG_PASSWORD2 "ssid3"
//Password for AP with SSID3
#define CONFIG_PASSWORD3 "ssid4"
//Password for AP with SSID4
#define CONFIG_PASSWORD4 "ssid4"
//Connection timeout in station mode in milliseconds. If board can't connect to one of defined netword, it will switch to Access Point mode
#define CONFIG_ST_CONNECTION_TIMEOUT 5000

class GrblCtrl_Config{
public:
    GrblCtrl_Config();
    void setInternalFS(FS* theFS);
    void setExternalFS(FS* theFS);
    void init();
    bool isAcessPoint() const;
    String getAccessPointName() const;
    String getAccessPointPassword() const;
    int getSSIDCount() const;
    String getSSID(int theIndx) const;
    String getSTPassword(int theIndx) const;
    int   getConnectionTimeout() const;
protected: 
    void fillDefaults();
    void readConfig(FS* theFS, String theFileName);
    void saveConfig();
    void fillConfigDoc(JsonDocument& theDoc);
protected:
    static const String m_sTemporaryFileName;
    static const String m_sMainConfigFileName;
    static const String m_sOldConfigFileName;        
    static const String m_sWiFiSection;
    static const String m_sWifiAPModeKey;
    static const String m_sAceessPointSection;
    static const String m_sWiFiAccessPointNameKey;
    static const String m_sWiFiAccessPointPwdKey;
    static const String m_sStationSection;
    static const String m_sNetworkSection;
    static const String m_sConnectionTimeoutKey;
    static const String m_sSSIDKey;
    static const String m_sStPWdKey;    
private:
    String m_sAccessPointName;
    String m_sAPPassword;
    String m_sSSID[CONFIG_STORED_SSID_COUNT];
    String m_sSTPWD[CONFIG_STORED_SSID_COUNT];
    bool m_isAP;
    int  m_iConnectionTimeout;
    FS*  m_InternalFS;
    FS*  m_ExternalFS;
};

#endif