#include <sys/stat.h>
#include <sys/unistd.h>
#include <errno.h>
#include <esp_log.h>
#include <string.h>


#include <cJSON.h>

#include "grbl_board_default_config.h"
#include "grbl_config_manager.h"
#include "grbl_utils.h"

#define MAX_JSON_BUFFER_SIZE    4096
#define INVALID_INDEX_MARK      255
#define INVALID_INDEX           -1

static uint8_t _m_IntIndexes[GRBL_INT_PARS_SIZE+1] = {0,1,2,3,10,23,26,INVALID_INDEX_MARK};
static uint8_t _m_BoolIndexes[GRBL_BOOL_PARS_SIZE+1] = {4,5,6,13,20,21,22,32,INVALID_INDEX_MARK};
static uint8_t _m_DoubleIndexes[GRBL_DOUBLE_PARS_SIZE+1] = {11,12,24,25,27,30,31,
                                                         100,101,102,103,104,105,106,107,
                                                         110,111,112,113,114,115,116,117,
                                                         120,121,122,123,124,125,126,127,
                                                         130,131,132,133,134,135,136,137,
                                                         INVALID_INDEX_MARK};

esp_err_t grbl_GetRealParIndex(uint8_t* theArray, int theIndx, int* theResIndx)
{
    int anIndx = 0;
    *theResIndx = INVALID_INDEX;
    while( theArray[anIndx] != INVALID_INDEX_MARK){
        if( theArray[anIndx] == theIndx ){
            *theResIndx = anIndx;
            return ESP_OK;
        }
        anIndx++;
    }
    return ESP_FAIL;
}

void grbl_InitModeConfig( GRBLBoardModeConfig* theConfig, bool isAP )
{
    if( isAP ){
        strcpy(theConfig->m_SSID,DEFAULT_AP_SSID);
        strcpy(theConfig->m_Password,DEFAULT_AP_PASSWORD);
        strcpy(theConfig->m_IP,DEFAULT_AP_IP);
        strcpy(theConfig->m_NetMask,DEFAULT_AP_NETMASK);
        strcpy(theConfig->m_Gateway,DEFAULT_AP_GATEWAY);
    }
    else{
        strcpy(theConfig->m_SSID,DEFAULT_ST_SSID);
        strcpy(theConfig->m_Password,DEFAULT_ST_PASSWORD);
        strcpy(theConfig->m_IP,DEFAULT_ST_IP);
        strcpy(theConfig->m_NetMask,DEFAULT_ST_NETMASK);
        strcpy(theConfig->m_Gateway,DEFAULT_ST_GATEWAY);
    }
}

void grbl_InitGRBLParameters( GRBLBoardConfig* theConfig )
{
    GRBL_SetGRBLParameterValue(theConfig,0,DEFAULT_GRBL_0);
    GRBL_SetGRBLParameterValue(theConfig,1,DEFAULT_GRBL_1);
    GRBL_SetGRBLParameterValue(theConfig,2,DEFAULT_GRBL_2);
    GRBL_SetGRBLParameterValue(theConfig,3,DEFAULT_GRBL_3);
    GRBL_SetGRBLParameterValue(theConfig,4,DEFAULT_GRBL_4);
    GRBL_SetGRBLParameterValue(theConfig,5,DEFAULT_GRBL_5);
    GRBL_SetGRBLParameterValue(theConfig,6,DEFAULT_GRBL_6);
    GRBL_SetGRBLParameterValue(theConfig,10,DEFAULT_GRBL_10);
    GRBL_SetGRBLParameterValue(theConfig,11,DEFAULT_GRBL_11);
    GRBL_SetGRBLParameterValue(theConfig,12,DEFAULT_GRBL_12);
    GRBL_SetGRBLParameterValue(theConfig,13,DEFAULT_GRBL_13);
    GRBL_SetGRBLParameterValue(theConfig,20,DEFAULT_GRBL_20);
    GRBL_SetGRBLParameterValue(theConfig,21,DEFAULT_GRBL_21);
    GRBL_SetGRBLParameterValue(theConfig,22,DEFAULT_GRBL_22);
    GRBL_SetGRBLParameterValue(theConfig,23,DEFAULT_GRBL_23);
    GRBL_SetGRBLParameterValue(theConfig,24,DEFAULT_GRBL_24);
    GRBL_SetGRBLParameterValue(theConfig,25,DEFAULT_GRBL_25);
    GRBL_SetGRBLParameterValue(theConfig,26,DEFAULT_GRBL_26);
    GRBL_SetGRBLParameterValue(theConfig,27,DEFAULT_GRBL_27);
    GRBL_SetGRBLParameterValue(theConfig,30,DEFAULT_GRBL_30);
    GRBL_SetGRBLParameterValue(theConfig,31,DEFAULT_GRBL_31);
    GRBL_SetGRBLParameterValue(theConfig,32,DEFAULT_GRBL_32);
    GRBL_SetGRBLParameterValue(theConfig,100,DEFAULT_GRBL_100);
    GRBL_SetGRBLParameterValue(theConfig,101,DEFAULT_GRBL_101);
    GRBL_SetGRBLParameterValue(theConfig,102,DEFAULT_GRBL_102);
    GRBL_SetGRBLParameterValue(theConfig,103,DEFAULT_GRBL_103);
    GRBL_SetGRBLParameterValue(theConfig,104,DEFAULT_GRBL_104);
    GRBL_SetGRBLParameterValue(theConfig,105,DEFAULT_GRBL_105);
    GRBL_SetGRBLParameterValue(theConfig,106,DEFAULT_GRBL_106);
    GRBL_SetGRBLParameterValue(theConfig,107,DEFAULT_GRBL_107);
    GRBL_SetGRBLParameterValue(theConfig,110,DEFAULT_GRBL_110);
    GRBL_SetGRBLParameterValue(theConfig,111,DEFAULT_GRBL_111);
    GRBL_SetGRBLParameterValue(theConfig,112,DEFAULT_GRBL_112);
    GRBL_SetGRBLParameterValue(theConfig,113,DEFAULT_GRBL_113);
    GRBL_SetGRBLParameterValue(theConfig,114,DEFAULT_GRBL_114);
    GRBL_SetGRBLParameterValue(theConfig,115,DEFAULT_GRBL_115);
    GRBL_SetGRBLParameterValue(theConfig,116,DEFAULT_GRBL_116);
    GRBL_SetGRBLParameterValue(theConfig,117,DEFAULT_GRBL_117);
    GRBL_SetGRBLParameterValue(theConfig,120,DEFAULT_GRBL_120);
    GRBL_SetGRBLParameterValue(theConfig,121,DEFAULT_GRBL_121);
    GRBL_SetGRBLParameterValue(theConfig,122,DEFAULT_GRBL_122);
    GRBL_SetGRBLParameterValue(theConfig,123,DEFAULT_GRBL_123);
    GRBL_SetGRBLParameterValue(theConfig,124,DEFAULT_GRBL_124);
    GRBL_SetGRBLParameterValue(theConfig,125,DEFAULT_GRBL_125);
    GRBL_SetGRBLParameterValue(theConfig,126,DEFAULT_GRBL_126);
    GRBL_SetGRBLParameterValue(theConfig,127,DEFAULT_GRBL_127);
    GRBL_SetGRBLParameterValue(theConfig,130,DEFAULT_GRBL_130);
    GRBL_SetGRBLParameterValue(theConfig,131,DEFAULT_GRBL_131);
    GRBL_SetGRBLParameterValue(theConfig,132,DEFAULT_GRBL_132);
    GRBL_SetGRBLParameterValue(theConfig,133,DEFAULT_GRBL_133);
    GRBL_SetGRBLParameterValue(theConfig,134,DEFAULT_GRBL_134);
    GRBL_SetGRBLParameterValue(theConfig,135,DEFAULT_GRBL_135);
    GRBL_SetGRBLParameterValue(theConfig,136,DEFAULT_GRBL_136);
    GRBL_SetGRBLParameterValue(theConfig,137,DEFAULT_GRBL_137);
}

esp_err_t GRBL_InitConfigManager(  GRBLBoardConfig* theConfig )
{
    theConfig->m_isAccessPoint = IS_AP_BY_DEFAULT;
    grbl_InitModeConfig(&theConfig->m_StConfig, false);
    grbl_InitModeConfig(&theConfig->m_APConfig, true);
    theConfig->m_StAttemptsCount = DEFAULT_CONNECTION_ATTEMPTS;
    grbl_InitGRBLParameters(theConfig);
    return ESP_OK;
}

esp_err_t GRBL_ConfigUpdateByJSON( GRBLBoardConfig* theConfig, char* theFileName)
{
/*    struct stat aFileStat;
    esp_err_t aRes;
    char*   aFileBuffer[MAX_JSON_BUFFER_SIZE];
    FILE*   aCfgFile;
    cJSON*  aCfgJson;
    int aRdCount;

    aRes = stat(theFileName, &aFileStat);
    if( aRes != ESP_OK ){
        ESP_LOGI(TAG,"Config file not found");
        return ESP_ERR_NOT_FOUND;
    }
    aCfgFile = fopen(theFileName, "r");
    if( !aCfgFile ){
        ESP_LOGE(TAG,"Can't open config file %s", theFileName);
        return ESP_FAIL;
    }
    aRdCount = fread(aFileBuffer, 1, aFileStat.st_size, aCfgFile);
    fclose(aCfgFile);
    if( aRdCount != aFileStat.st_size ){
        ESP_LOGE(TAG,"Reading config file error.");
        return ESP_FAIL;
    }
    aCfgJson = cJSON_Parse((const char*)aFileBuffer);
    if( !aCfgJson ){
        const char* anErrorPtr = cJSON_GetErrorPtr();
        if (anErrorPtr != NULL){
            ESP_LOGE(TAG,"Error parse json. %s", anErrorPtr);
        }
        else{
            ESP_LOGE(TAG,"Can't parse JSON config");
        }
        return ESP_FAIL; 
    }

    cJSON_Delete(aCfgJson);*/
    return ESP_OK;    
}

esp_err_t GRBL_SetConfigStringParameter(char* theTarget, char* theValue, int theSize)
{
    int aValLen = strlen(theValue);
    if( aValLen > theSize ){
        ESP_LOGE(TAG,"Parameter is too long");
        return ESP_FAIL;
    }
    strncpy(theTarget, theValue, aValLen);
    theTarget[aValLen] = 0;
    return ESP_OK;
}

int GRBL_GetSDMOSIPin( GRBLBoardConfig* theConfig )
{
    return DEFAULT_PIN_NUM_MOSI; 
}

int GRBL_GetSDMISOPin( GRBLBoardConfig* theConfig )
{
    return DEFAULT_PIN_NUM_MISO;
}

int GRBL_GetSDSCKPin( GRBLBoardConfig* theConfig )
{
    return DEFAULT_PIN_NUM_CLK;
}

int GRBL_GetSDCSPin(  GRBLBoardConfig* theConfig )
{
    return DEFAULT_PIN_NUM_CS_SD;
}

int GRBL_GetSDDetPin( GRBLBoardConfig* theConfig )
{
    return DEFAULT_PIN_CARD_DET;
}

char* GRBL_GetInternalFlashPrefix( GRBLBoardConfig* theConfig )
{
    return INTERNAL_FLASH_BASE_PATH;
}

char* GRBL_GetSDPrefix(  GRBLBoardConfig* theConfig )
{
    return SD_BASE_PATH;
}

/****************************** WiFi parameters **************************/
bool GRBL_IsWiFiAP( GRBLBoardConfig* theConfig )
{
    return theConfig->m_isAccessPoint;
}

void GRBL_SetIsWiFiAP( GRBLBoardConfig* theConfig, bool isAP )
{
    theConfig->m_isAccessPoint = isAP;
}

char* GRBL_GetSSID( GRBLBoardConfig* theConfig, bool isAP )
{
    if( isAP ){
        return theConfig->m_APConfig.m_SSID;
    }
    return theConfig->m_StConfig.m_SSID;
}

esp_err_t GRBL_SetSSID( GRBLBoardConfig* theConfig, char* theValue, bool isAP )
{
    char* aRes = theConfig->m_StConfig.m_SSID;
    if( isAP ){
        aRes = theConfig->m_APConfig.m_SSID;
    }
    return GRBL_SetConfigStringParameter(aRes,theValue,MAX_SSID_SIZE);
}

char* GRBL_GetPassword( GRBLBoardConfig* theConfig, bool isAP )
{
    if( isAP ){
        return theConfig->m_APConfig.m_Password;
    }
    return theConfig->m_StConfig.m_Password;
}

esp_err_t GRBL_SetPassword( GRBLBoardConfig* theConfig, char* theValue, bool isAP )
{
    char* aRes = theConfig->m_StConfig.m_Password;
    if( isAP ){
        aRes = theConfig->m_APConfig.m_Password;
    }
    return GRBL_SetConfigStringParameter(aRes,theValue,MAX_PASSWORD_SIZE);
}

char* GRBL_GetIP( GRBLBoardConfig* theConfig, bool isAP )
{
    if( isAP ){
        return theConfig->m_APConfig.m_IP;
    }
    return theConfig->m_StConfig.m_IP;
}

esp_err_t GRBL_SetIP( GRBLBoardConfig* theConfig, char* theValue, bool isAP )
{
    char* aRes = theConfig->m_StConfig.m_IP;
    if( isAP ){
        aRes = theConfig->m_APConfig.m_IP;
    }
    return GRBL_SetConfigStringParameter(aRes,theValue,MAX_IP_SIZE);
}

esp_err_t GRBL_SetNetMask( GRBLBoardConfig* theConfig, char* theValue, bool isAP )
{
    char* aRes = theConfig->m_StConfig.m_NetMask;
    if( isAP ){
        aRes = theConfig->m_APConfig.m_NetMask;
    }
    return GRBL_SetConfigStringParameter(aRes,theValue,MAX_IP_SIZE);
}

char* GRBL_GetNetMask( GRBLBoardConfig* theConfig, bool isAP )
{
    if( isAP ){
        return theConfig->m_APConfig.m_NetMask;
    }
    return theConfig->m_StConfig.m_NetMask;
}

char* GRBL_GetGateway( GRBLBoardConfig* theConfig, bool isAP )
{
    if( isAP ){
        return theConfig->m_APConfig.m_Gateway;
    }
    return theConfig->m_StConfig.m_Gateway;
}

esp_err_t GRBL_SetGateway( GRBLBoardConfig* theConfig, char* theValue, bool isAP )
{
    char* aRes = theConfig->m_StConfig.m_Gateway;
    if( isAP ){
        aRes = theConfig->m_APConfig.m_Gateway;
    }
    return GRBL_SetConfigStringParameter(aRes,theValue,MAX_IP_SIZE);
}

int GRBL_GetStAttempts( GRBLBoardConfig* theConfig )
{
    return theConfig->m_StAttemptsCount;
}

esp_err_t GRBL_SetStAttempts( GRBLBoardConfig* theConfig, int theAttempts)
{
    theConfig->m_StAttemptsCount = theAttempts;
    return ESP_OK;
}

/****************************** GRBL parameters **************************/
GRBL_ParameterType GRBL_GetGRBLParamType(int theIndx, int* theRealIndx)
{
    esp_err_t aRetCode;
    aRetCode = grbl_GetRealParIndex(_m_IntIndexes,theIndx,theRealIndx);
    if( aRetCode == ESP_OK ){
        return GPT_INT;
    }
    aRetCode = grbl_GetRealParIndex(_m_BoolIndexes,theIndx,theRealIndx);
    if( aRetCode == ESP_OK ){
        return GPT_BOOL;
    }
    aRetCode = grbl_GetRealParIndex(_m_DoubleIndexes,theIndx,theRealIndx);
    if( aRetCode == ESP_OK ){
        return GPT_DOUBLE;
    }
    return GPT_INVALID;
}

esp_err_t GRBL_GetGRBLParamBool( GRBLBoardConfig* theConfig, int theIndx, bool* theRes)
{
    esp_err_t aRetCode;
    int anIndx = INVALID_INDEX;
    aRetCode = grbl_GetRealParIndex(_m_BoolIndexes,theIndx,&anIndx);
    if( aRetCode != ESP_OK )
        return aRetCode;
    *theRes = theConfig->m_ParsBool[anIndx];
    return ESP_OK;
}

esp_err_t GRBL_GetGRBLParamInt( GRBLBoardConfig* theConfig, int theIndx, unsigned int* theRes )
{
    esp_err_t aRetCode;
    int anIndx = INVALID_INDEX;
    aRetCode = grbl_GetRealParIndex(_m_IntIndexes,theIndx,&anIndx);
    if( aRetCode != ESP_OK )
        return aRetCode;
    *theRes = theConfig->m_ParsInt[anIndx];
    return ESP_OK;    
}

esp_err_t GRBL_GetGRBLParamDouble( GRBLBoardConfig* theConfig, int theIndx, double* theRes)
{
    esp_err_t aRetCode;
    int anIndx = INVALID_INDEX;
    aRetCode = grbl_GetRealParIndex(_m_DoubleIndexes,theIndx,&anIndx);
    if( aRetCode != ESP_OK )
        return aRetCode;
    *theRes = theConfig->m_ParsDouble[anIndx];
    return ESP_OK;
}

esp_err_t GRBL_SetGRBLParameterValue( GRBLBoardConfig* theConfig, int theIndx, char* theString)
{
    int aRealIndx;
    esp_err_t aRetCode;
    GRBL_ParameterType aParType = GRBL_GetGRBLParamType(theIndx, &aRealIndx);
    switch(aParType){
        case GPT_INVALID:
            return ESP_FAIL;
        case GPT_INT:{
            unsigned int aVal;
            aRetCode = GRBL_ConvertDigit(theString, &aVal);
            if( aRetCode != ESP_OK )
                return aRetCode;
            theConfig->m_ParsInt[aRealIndx] = aVal;
            return ESP_OK;
        }
        break;
        case GPT_DOUBLE:{
            double aVal;
            int aCnt = sscanf(theString,"%lg",&aVal);
            if( aCnt != 1 ){
                return ESP_FAIL;
            }
            theConfig->m_ParsDouble[aRealIndx] = aVal;
            return ESP_OK;
        }
        break;
        case GPT_BOOL:{
            bool aVal = false;
            if( strcmp(theString, ON_VALUE) == 0 ){
                aVal = true;
            }
            else if( strcmp(theString, OFF_VALUE) == 0){
                aVal = false;
            }
            else{
                return ESP_FAIL;
            }
            theConfig->m_ParsBool[aRealIndx] = aVal;
            return ESP_OK;
        }
        break;
    }
    return ESP_FAIL;
}

esp_err_t GRBL_GetGRBLParameterValue( GRBLBoardConfig* theConfig, int theIndx, char* theBuffer)
{
    theBuffer[0] = 0;
    int aRealIndx;
    GRBL_ParameterType aParType = GRBL_GetGRBLParamType(theIndx, &aRealIndx);
    switch(aParType){
        case GPT_INVALID:
            return ESP_FAIL;
        case GPT_BOOL:{
            bool aVal = theConfig->m_ParsBool[aRealIndx];
            if( aVal ){
                strcpy(theBuffer,ON_VALUE);
            }
            else{
                strcpy(theBuffer,OFF_VALUE);
            }
            return ESP_OK;
        }
        break;
        case GPT_INT:{
            int aVal = theConfig->m_ParsInt[aRealIndx];
            sprintf(theBuffer,"%d", aVal);
            return ESP_OK;
        }
        break;
        case GPT_DOUBLE:{
            double aVal = theConfig->m_ParsDouble[aRealIndx];
            sprintf(theBuffer,"%lg", aVal);
            return ESP_OK;
        }
        break;
    }
    return ESP_FAIL;
}
