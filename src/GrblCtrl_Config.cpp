#include "GrblCtrl_Config.h"
#include "GrblCtrl_Debug.h"
#include <ArduinoJson.h>

#define JSON_DOC_SIZE 400

const String GrblCtrl_Config::m_sMainConfigFileName="config.json";        
const String GrblCtrl_Config::m_sTemporaryFileName="config_tmp.json";
const String GrblCtrl_Config::m_sOldConfigFileName="config.json.old";        

const String GrblCtrl_Config::m_sWiFiSection="wifi";
const String GrblCtrl_Config::m_sWifiAPModeKey="access_point_mode";
const String GrblCtrl_Config::m_sAceessPointSection="access_point";
const String GrblCtrl_Config::m_sWiFiAccessPointNameKey="name";
const String GrblCtrl_Config::m_sWiFiAccessPointPwdKey="password";
const String GrblCtrl_Config::m_sStationSection="station";
const String GrblCtrl_Config::m_sNetworkSection="network";
const String GrblCtrl_Config::m_sSSIDKey="ssid";
const String GrblCtrl_Config::m_sStPWdKey="password";


GrblCtrl_Config::GrblCtrl_Config() : 
    m_InternalFS(NULL), m_ExternalFS(NULL)
{
   fillDefaults(); 
}

void GrblCtrl_Config::setInternalFS(FS* theFS)
{
    m_InternalFS = theFS;
}

void GrblCtrl_Config::setExternalFS(FS* theFS)
{
    m_ExternalFS = theFS;
}

void GrblCtrl_Config::init()
{
    DEBUG_PRINT_LN("");
    DEBUG_PRINT_LN("GrblCtrl_Config::init");
    if( m_InternalFS != NULL ){
//Power loss recovery        
        if( !m_InternalFS->exists(m_sMainConfigFileName) ){
            if( m_InternalFS->exists(m_sTemporaryFileName) ){
                m_InternalFS->rename(m_sTemporaryFileName, m_sMainConfigFileName);
            }
        }
        if( m_InternalFS->exists(m_sMainConfigFileName)){
//Read config from internal file system            
            DEBUG_PRINT_LN("Internal config file is found");
            readConfig(m_InternalFS, m_sMainConfigFileName);
        }
        else{
            DEBUG_PRINT_LN("Internal config file not found");
        }
    }
    if( m_ExternalFS != NULL ){
        if( m_ExternalFS->exists(m_sMainConfigFileName) ){
            DEBUG_PRINT_LN("External config file found");
            readConfig(m_ExternalFS, m_sMainConfigFileName);
            if( m_InternalFS != NULL ){
                saveConfig();
            }
//            m_ExternalFS->rename(m_sMainConfigFileName,m_sOldConfigFileName);
        }
    }
}

void GrblCtrl_Config::fillDefaults()
{
//Switch on Access point by default    
    m_isAP = CONFIG_INITIAL_AP_MODE;
//Access point name     
    m_sAccessPointName = CONFIG_AP_NAME;
//Access point password    
    m_sAPPassword = CONFIG_AP_PASSWORD;
//Stations ssid and passwords    
    for( int i = 0 ; i < CONFIG_STORED_SSID_COUNT ; i++ ){
       m_sSSID[0] = "";
       m_sSTPWD[0]  = ""; 
    }
//Stations name and passwords    
    m_sSSID[0] = CONFIG_SSID1;
    m_sSTPWD[0] = CONFIG_PASSWORD1;
    m_sSSID[1] = CONFIG_SSID2;
    m_sSTPWD[1] = CONFIG_PASSWORD2;
    m_sSSID[2] = CONFIG_SSID3;
    m_sSTPWD[2] = CONFIG_PASSWORD3;
    m_sSSID[3] = CONFIG_SSID4;
    m_sSTPWD[3] = CONFIG_PASSWORD4;
//Station mode connection timeout
    m_iConnectionTimeout = CONFIG_ST_CONNECTION_TIMEOUT;     
}

void GrblCtrl_Config::readConfig(FS* theFS, String theFileName)
{
    if( theFS == NULL )
        return;
    File aJsonFile = theFS->open(theFileName, "r");
    if(!aJsonFile){
        return;
    }
    StaticJsonDocument<JSON_DOC_SIZE> aConfigDoc;
    DeserializationError anErr = deserializeJson(aConfigDoc, aJsonFile);
    if( anErr ){
        DEBUG_PRINT("Error deserialize JSON. File ");
        DEBUG_PRINT_LN(theFileName);
        DEBUG_PRINT_LN(anErr.c_str());
        return;
    }
    aJsonFile.close();
    JsonVariant anAPVar = aConfigDoc[m_sWiFiSection][m_sWifiAPModeKey];
    if( !anAPVar.isNull() ){
        DEBUG_PRINT_LN("AP mode found");
        m_isAP = anAPVar.as<bool>();       
    }
    JsonVariant anAPNameVar = aConfigDoc[m_sWiFiSection][m_sAceessPointSection][m_sWiFiAccessPointNameKey];
    if( !anAPNameVar.isNull() ){
        DEBUG_PRINT_LN("AP name found");
        m_sAccessPointName = anAPNameVar.as<String>();       
    }
    JsonVariant anAPPwdVar = aConfigDoc[m_sWiFiSection][m_sAceessPointSection][m_sWiFiAccessPointPwdKey];
    if( !anAPPwdVar.isNull() ){
        DEBUG_PRINT_LN("AP password found");
        m_sAPPassword = anAPPwdVar.as<String>();       
    }
    for( int i = 0 ; i < CONFIG_STORED_SSID_COUNT ; i++ ){
        JsonVariant aSSIDVar = aConfigDoc[m_sWiFiSection][m_sStationSection][m_sNetworkSection][i][m_sSSIDKey];
        if( !aSSIDVar.isNull() ){
            m_sSSID[i] = aSSIDVar.as<String>();
            DEBUG_PRINT("SSID found ");
            DEBUG_PRINT_LN(m_sSSID[i]);
        }
        JsonVariant aPwdVar = aConfigDoc[m_sWiFiSection][m_sStationSection][m_sNetworkSection][i][m_sStPWdKey];
        if( !aPwdVar.isNull() ){
            m_sSTPWD[i] = aPwdVar.as<String>();
            DEBUG_PRINT("Station password found ");
            DEBUG_PRINT_LN(m_sSTPWD[i]);
        }
    }
}

void GrblCtrl_Config::saveConfig()
{
    if( m_InternalFS == NULL )
        return;
    File aJsonFile = m_InternalFS->open(m_sTemporaryFileName, "w");
    if( !aJsonFile ){
        DEBUG_PRINT("Error. Can't create temporary config file ");
        DEBUG_PRINT_LN(m_sTemporaryFileName);
        return;
    }
    StaticJsonDocument<JSON_DOC_SIZE> aConfigDoc;
    fillConfigDoc(aConfigDoc);
    size_t aWrSize = serializeJson(aConfigDoc, aJsonFile);
    aJsonFile.close();
    if( aWrSize == 0 ){
        DEBUG_PRINT("Error write JSON. File ");
        DEBUG_PRINT_LN(m_sTemporaryFileName);
        return;
    }
    bool aRes = m_InternalFS->remove(m_sMainConfigFileName);
    if( !aRes ){
        DEBUG_PRINT_LN("Error remove old config file.");
        return;
    }
    aRes = m_InternalFS->rename(m_sTemporaryFileName, m_sMainConfigFileName);
    if( !aRes ){
        DEBUG_PRINT_LN("Error rename temporary config file");
        return;
    }
}

void GrblCtrl_Config::fillConfigDoc(JsonDocument& theDoc)
{
    theDoc[m_sWiFiSection][m_sWifiAPModeKey] = isAcessPoint();
    theDoc[m_sWiFiSection][m_sAceessPointSection][m_sWiFiAccessPointNameKey] = getAccessPointName();
    theDoc[m_sWiFiSection][m_sAceessPointSection][m_sWiFiAccessPointPwdKey] = getAccessPointPassword();
    for( int i = 0 ; i < CONFIG_STORED_SSID_COUNT ; i++ ){
        theDoc[m_sWiFiSection][m_sStationSection][m_sNetworkSection][i][m_sSSIDKey] = getSSID(i);
        theDoc[m_sWiFiSection][m_sStationSection][m_sNetworkSection][i][m_sStPWdKey] = getSTPassword(i);
    }
}

bool GrblCtrl_Config::isAcessPoint() const
{
    return m_isAP;
}

String GrblCtrl_Config::getAccessPointName() const
{
    return String(m_sAccessPointName);
}

String GrblCtrl_Config::getAccessPointPassword() const
{
    return String(m_sAPPassword);
}

int GrblCtrl_Config::getSSIDCount() const
{
    return CONFIG_STORED_SSID_COUNT;
}

String GrblCtrl_Config::getSSID(int theIndx) const
{
    if( theIndx < 0 || theIndx > CONFIG_STORED_SSID_COUNT )
        return String();
    Serial.println(theIndx);
    Serial.println(m_sSSID[theIndx]);
    return m_sSSID[theIndx];
}

String GrblCtrl_Config::getSTPassword(int theIndx) const
{
    if( theIndx < 0 || theIndx > CONFIG_STORED_SSID_COUNT )
        return String();
    return m_sSTPWD[theIndx];
}

int GrblCtrl_Config::getConnectionTimeout() const
{
    return m_iConnectionTimeout;
}
