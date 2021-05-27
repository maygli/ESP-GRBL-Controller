#include "config.h"

void ControllerConfig::fillDefaults()
{
//Switch on Access point by default    
    m_isAP = CONFIG_INITIAL_AP_MODE;
//Access point name     
    strcpy( m_sAPAppPointName, CONFIG_AP_NAME);
//Access point password    
    strcpy( m_sAPPassword, CONFIG_AP_PASSWORD);
//Stations ssid and passwords    
    for( int i = 0 ; i < CONFIG_STORED_SSID_COUNT ; i++ ){
       m_sSSID[0][0] = 0 ;
       m_sSTPWD[0][0]  = 0; 
    }
//Stations name and passwords    
    strcpy( m_sSSID[0], CONFIG_SSID1);
    strcpy( m_sSTPWD[0],CONFIG_PASSWORD1);
    strcpy( m_sSSID[1], CONFIG_SSID2);
    strcpy( m_sSTPWD[1],CONFIG_PASSWORD2);
    strcpy( m_sSSID[2], CONFIG_SSID3);
    strcpy( m_sSTPWD[2],CONFIG_PASSWORD3);
    strcpy( m_sSSID[3], CONFIG_SSID4);
    strcpy( m_sSTPWD[3],CONFIG_PASSWORD4);
//Station mode connection timeout
    m_iConnectionTimeout = CONFIG_ST_CONNECTION_TIMEOUT;     
}

ControllerConfig::ControllerConfig()
{
   fillDefaults(); 
}

bool ControllerConfig::isAcessPoint() const
{
    return m_isAP;
}

String ControllerConfig::getAccessPointName() const
{
    return String(m_sAPAppPointName);
}

String ControllerConfig::getAccessPointPassword() const
{
    return String(m_sAPPassword);
}

int ControllerConfig::getSSIDCount() const
{
    return CONFIG_STORED_SSID_COUNT;
}

String ControllerConfig::getSSID(int theIndx) const
{
    if( theIndx < 0 || theIndx > CONFIG_STORED_SSID_COUNT )
        return String();
    Serial.println(theIndx);
    Serial.println(m_sSSID[theIndx]);
    return m_sSSID[theIndx];
}

String ControllerConfig::getSTPassword(int theIndx) const
{
    if( theIndx < 0 || theIndx > CONFIG_STORED_SSID_COUNT )
        return String();
    return m_sSTPWD[theIndx];
}

int ControllerConfig::getConnectionTimeout() const
{
    return m_iConnectionTimeout;
}
