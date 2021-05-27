#ifndef CONFIG_H
#define CONFIG_H

#include <LittleFS.h>

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

class ControllerConfig{
public:
    ControllerConfig();
    bool isAcessPoint() const;
    String getAccessPointName() const;
    String getAccessPointPassword() const;
    int getSSIDCount() const;
    String getSSID(int theIndx) const;
    String getSTPassword(int theIndx) const;
    int   getConnectionTimeout() const;
protected: 
    void fillDefaults();    
private:
    char m_sAPAppPointName[64];
    char m_sAPPassword[64];
    char m_sSSID[CONFIG_STORED_SSID_COUNT][64];
    char m_sSTPWD[CONFIG_STORED_SSID_COUNT][64];
    bool m_isAP;
    int  m_iConnectionTimeout;    
};

#endif