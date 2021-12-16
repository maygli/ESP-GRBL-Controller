#include <sys/param.h>
#include <esp_err.h>
#include <esp_log.h>
#include <string.h>
#include <stdio.h>

#include <ini.h>

#include "grbl_config_manager.h"
#include "grbl_board_default_config.h"
#include "grbl_utils.h"

#define MAX_PAR_BUFFER_SIZE     32
#define SECTION_WIFI            "wifi"
#define SECTION_WIFI_STATION    "wifi.station"
#define SECTION_WIFI_AP         "wifi.access_point"
#define SECTION_GRBL            "grbl"
#define KEYWORD_MODE            "mode"
#define KEYWORD_SSID            "ssid"
#define KEYWORD_PASSWORD        "password"
#define KEYWORD_IP              "ip"
#define KEYWORD_NETMASK         "net_mask"
#define KEYWORD_GATEWAY         "gateway"
#define KEYWORD_ATTEMPTS        "attempts"
#define VAL_STATION_MODE        "sta"
#define VAL_AP_MODE             "ap"

esp_err_t grbl_saveIniModeSection( FILE* theFile, GRBLBoardModeConfig* theConfig, char* theSection )
{
    fprintf(theFile,"[%s]\r\n", theSection);
    fprintf(theFile, "%s=%s\r\n", KEYWORD_SSID, theConfig->m_SSID);
    fprintf(theFile, "%s=%s\r\n", KEYWORD_PASSWORD, theConfig->m_Password);
    fprintf(theFile, "%s=%s\r\n", KEYWORD_IP, theConfig->m_IP);
    fprintf(theFile, "%s=%s\r\n", KEYWORD_NETMASK, theConfig->m_NetMask);
    fprintf(theFile, "%s=%s\r\n", KEYWORD_GATEWAY, theConfig->m_Gateway);
    return ESP_OK;
}

int grbl_setModeParameter( GRBLBoardModeConfig* theConfig, const char* theName, const char* theValue )
{
    int aValLen = strlen(theValue);
    int aRealLen = aValLen;
    int aRes=0;
    if( strcmp(KEYWORD_SSID, theName) == 0 ){
        GRBL_SetConfigStringParameter(theConfig->m_SSID,theValue,MAX_SSID_SIZE-1);
    }
    else if( strcmp(KEYWORD_PASSWORD, theName) == 0 ){
        GRBL_SetConfigStringParameter(theConfig->m_Password,theValue,MAX_PASSWORD_SIZE-1);
    }
    else if( strcmp(KEYWORD_IP, theName) == 0 ){
        GRBL_SetConfigStringParameter(theConfig->m_IP,theValue,MAX_IP_SIZE-1);
    }
    else if( strcmp(KEYWORD_NETMASK, theName) == 0 ){
        GRBL_SetConfigStringParameter(theConfig->m_NetMask,theValue,MAX_IP_SIZE-1);
    }
    else if( strcmp(KEYWORD_GATEWAY, theName) == 0 ){
        GRBL_SetConfigStringParameter(theConfig->m_Gateway,theValue,MAX_IP_SIZE-1);
    }
    return 0;
}

int grbl_OnIniParameter(void* user, const char* section, const char* name, const char* value)
{
    esp_err_t aRes;
    bool isNeedSave = false;
    GRBLBoardConfig* aConfig = (GRBLBoardConfig*)user;
    ESP_LOGI(TAG,"GRBL_ConfigUpdateByINI::Ini parameter. Section=%s, Parameter=%s, Value=%s", section, name, value);
    if( strcmp(SECTION_WIFI, section) == 0 ){
        if( strcmp(KEYWORD_MODE, name) == 0 ){
            bool isAP = true;
            if( strcmp(VAL_STATION_MODE, value) == 0 ){
                isAP = false;
                ESP_LOGI(TAG,"  GRBL_ConfigUpdateByINI::station mode");
            }
            aConfig->m_isAccessPoint = isAP;
            isNeedSave = true;
            return 1;
        }
        else{
            ESP_LOGE(TAG, "Error. Unknown parameter %s in section %s", name, section);
        }
    }
    else if( strcmp(SECTION_WIFI_STATION,section) == 0 ){
        if( strcmp(KEYWORD_ATTEMPTS, name) == 0 ){
        }
        else{
            grbl_setModeParameter(&aConfig->m_StConfig, name, value);
        }
    }
    else if( strcmp(SECTION_WIFI_AP,section) == 0 ){
        grbl_setModeParameter(&aConfig->m_APConfig, name, value);
    }
    else if( strcmp(SECTION_GRBL,section) == 0 ){
        ESP_LOGI(TAG,"Grbl section, Parameter=%s, Value=%s", name, value);
        int anArgNum;
        unsigned int aVal=0;
        int aCnt = sscanf(name,"%d", &anArgNum);
        if( aCnt != 1 ){
            ESP_LOGE(TAG,"Error in GRBL parameter conversion. Parameter '%s'", name);
            return 0;
        }
        aRes = GRBL_SetGRBLParameterValue(aConfig, anArgNum, value);
        if( aRes != ESP_OK )
            return 0;
    }
    return 1;
}

esp_err_t GRBL_ConfigUpdateByINI( GRBLBoardConfig* theConfig, char* theFileName)
{
    ESP_LOGI(TAG,"GRBL_ConfigUpdateByINI %s", theFileName);
    FILE* anIniFile = NULL;
    anIniFile = fopen(theFileName,"r");
    if( !anIniFile ){
        ESP_LOGI(TAG,"GRBL_ConfigUpdateByINI::Can't open file");
        return ESP_FAIL;
    }
    int aRes = ini_parse_file(anIniFile,&grbl_OnIniParameter,theConfig);
    fclose(anIniFile);
    if( aRes != 0 ){
        ESP_LOGI(TAG,"GRBL_ConfigUpdateByINI::Error parse ini file %d", aRes);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t GRBL_ConfigSaveINI( GRBLBoardConfig* theConfig, char* theFileName)
{
    ESP_LOGI(TAG,"GRBL_ConfigSaveINI %s", theFileName);
    FILE* anIniFile = NULL;
    anIniFile = fopen(theFileName,"w");
    if( !anIniFile ){
        ESP_LOGE(TAG,"GRBL_ConfigSaveINI::Can't open file for write INI");
        return ESP_FAIL;
    }
    fprintf(anIniFile,"[%s]\r\n", SECTION_WIFI);
    if( theConfig->m_isAccessPoint ){
        fprintf(anIniFile,"%s=%s\r\n", KEYWORD_MODE, VAL_AP_MODE);
    }
    else{
        fprintf(anIniFile,"%s=%s\r\n", KEYWORD_MODE, VAL_STATION_MODE);
    }
    grbl_saveIniModeSection(anIniFile,&theConfig->m_StConfig,SECTION_WIFI_STATION);
    fprintf(anIniFile,"%s=%d\r\n", KEYWORD_ATTEMPTS, theConfig->m_StAttemptsCount);
    grbl_saveIniModeSection(anIniFile,&theConfig->m_APConfig,SECTION_WIFI_AP);
    fprintf(anIniFile,"[%s]\r\n", SECTION_GRBL);
    char aBuffer[MAX_PAR_BUFFER_SIZE];
    for( int i = 0 ; i < GRBL_PARS_MAX_INDEX; i++){
        esp_err_t aRes = GRBL_GetGRBLParameterValue(theConfig,i,aBuffer);
        if( aRes == ESP_OK ){
            fprintf(anIniFile,"%d=", i);
            fputs(aBuffer, anIniFile);
            fprintf(anIniFile,"\r\n");
        }
    }
    fclose(anIniFile);
    return ESP_OK;
}