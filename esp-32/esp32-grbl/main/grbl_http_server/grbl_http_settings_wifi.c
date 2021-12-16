#include <esp_err.h>
#include <esp_log.h>
#include <esp_http_server.h>

#include <cJSON.h>

#include "../grbl_config_manager.h"
#include "grbl_http_server.h"
#include "grbl_http_utils.h"

#define TYPE_SELECTOR "type_selector"
#define AP_VAL "ap"
#define ST_VAL "st"

#define AP_GROUP "ap_"
#define ST_GROUP "st_"
#define PREFIX_NAME_LEN 3

#define SSID_PARAM      "ssid"
#define PASSWORD_PARAM  "pwd"
#define IP_PARAM        "ip"
#define NETMASK_PARAM   "netmask"
#define GATEWAY_PARAM   "gateway"
#define ATTEMPTS_PARAM  "attempts"

#define NAME_BUFFER_SIZE 16
#define VAL_BUFFER_SIZE  12

esp_err_t grbl_SendParameter(httpd_req_t *req, char* theName, char* theValue, bool isLast )
{
    httpd_resp_send_chunk(req,"\"",1);
    httpd_resp_send_chunk(req,theName,strlen(theName));
    httpd_resp_send_chunk(req,"\":\"",3);
    httpd_resp_send_chunk(req,theValue,strlen(theValue));
    httpd_resp_send_chunk(req,"\"",1);
    if( !isLast ){
      httpd_resp_send_chunk(req,",",1);
    }
    return ESP_OK;
}

void grbl_GetParamFullName(char* theBuffer, char* theParamName)
{
  strcpy(theBuffer+PREFIX_NAME_LEN, theParamName);
}

char* grbl_GetParamValue(cJSON* theJson, char* theNameBuffer, char* theParam)
{
  grbl_GetParamFullName(theNameBuffer, theParam);
  cJSON* aJson = cJSON_GetObjectItem(theJson,theNameBuffer);
  if( aJson ){
    char* aVal = cJSON_GetStringValue(aJson);
    return aVal;
  }
  return NULL;
}

esp_err_t grbl_SetGroupData(cJSON* theJson, GRBLBoardConfig* theConfig, bool isAP)
{
  esp_err_t aRes;
  char aNameBuffer[NAME_BUFFER_SIZE];
  char* aPrefix = ST_GROUP;
  if( isAP ){
    aPrefix = AP_GROUP;
  }
  strcpy( aNameBuffer, aPrefix );
  char* aVal = grbl_GetParamValue(theJson,aNameBuffer,SSID_PARAM);
  if( aVal ){
    aRes = GRBL_SetSSID(theConfig,aVal,isAP);
    if( aRes != ESP_OK )
      return aRes;
  }
  aVal = grbl_GetParamValue(theJson,aNameBuffer,PASSWORD_PARAM);
  if( aVal ){
    aRes = GRBL_SetPassword(theConfig,aVal,isAP);
    if( aRes != ESP_OK )
      return aRes;
  }
  aVal = grbl_GetParamValue(theJson,aNameBuffer,IP_PARAM);
  if( aVal ){
    aRes = GRBL_SetIP(theConfig,aVal,isAP);
    if( aRes != ESP_OK )
      return aRes;
  }
  aVal = grbl_GetParamValue(theJson,aNameBuffer,NETMASK_PARAM);
  if( aVal ){
    aRes = GRBL_SetNetMask(theConfig,aVal,isAP);
    if( aRes != ESP_OK )
      return aRes;
  }
  aVal = grbl_GetParamValue(theJson,aNameBuffer,GATEWAY_PARAM);
  if( aVal ){
    aRes = GRBL_SetGateway(theConfig,aVal,isAP);
    if( aRes != ESP_OK )
      return aRes;
  }
  if( !isAP ){
    aVal = grbl_GetParamValue(theJson,aNameBuffer,ATTEMPTS_PARAM);
    if( aVal ){
      int anAttempts = 0;
      int aConv = sscanf(aVal,"%d",&anAttempts);
      if( aConv != 1 )
        return ESP_FAIL;
      GRBL_SetStAttempts(theConfig, anAttempts);
    }
  }
  return ESP_OK;
}

esp_err_t grbl_SendGroupData(httpd_req_t *req, GRBLBoardConfig* theConfig, bool isAP)
{
  char* aVal;
  char aNameBuffer[NAME_BUFFER_SIZE];
  char* aPrefix = ST_GROUP;
  if( isAP ){
    aPrefix = AP_GROUP;
  }
  strcpy( aNameBuffer, aPrefix );

  grbl_GetParamFullName(aNameBuffer, SSID_PARAM);
  aVal = GRBL_GetSSID(theConfig,isAP);
  grbl_SendParameter(req, aNameBuffer, aVal, false );
  grbl_GetParamFullName(aNameBuffer, PASSWORD_PARAM);
  aVal = GRBL_GetPassword(theConfig,isAP);
  grbl_SendParameter(req, aNameBuffer, aVal, false );
  grbl_GetParamFullName(aNameBuffer, IP_PARAM);
  aVal = GRBL_GetIP(theConfig,isAP);
  grbl_SendParameter(req, aNameBuffer, aVal, false );
  grbl_GetParamFullName(aNameBuffer, NETMASK_PARAM);
  aVal = GRBL_GetNetMask(theConfig,isAP);
  grbl_SendParameter(req, aNameBuffer, aVal, false );
  grbl_GetParamFullName(aNameBuffer, GATEWAY_PARAM);
  aVal = GRBL_GetGateway(theConfig,isAP);
  grbl_SendParameter(req, aNameBuffer, aVal, false );
  if( !isAP ){
    char aValBuffer[VAL_BUFFER_SIZE];
    int aVal = GRBL_GetStAttempts(theConfig);
    sprintf(aValBuffer,"%d",aVal);
    grbl_GetParamFullName(aNameBuffer, ATTEMPTS_PARAM);
    grbl_SendParameter(req, aNameBuffer, aValBuffer, false );
  }
  return ESP_OK;
}

esp_err_t GRBL_SendSettingsWiFi(httpd_req_t *req)
{
    GRBLServer* aServer = (GRBLServer*)req->user_ctx;
    GRBLBoardConfig* aConfig = aServer->m_Config;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send_chunk(req,"{",1);
    char* aTypeSelVal = AP_VAL;
    if( !aConfig->m_isAccessPoint ){
      aTypeSelVal = ST_VAL;
    }
    grbl_SendGroupData(req, aConfig, false );
    grbl_SendGroupData(req, aConfig, true );
    grbl_SendParameter(req,TYPE_SELECTOR,aTypeSelVal, true);
    httpd_resp_send_chunk(req,"}",1);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t GRBL_SetSettingsWiFi(httpd_req_t *req)
{
  esp_err_t aRetVal;
  GRBLServer* aServer = (GRBLServer*)req->user_ctx;
  GRBLBoardConfig* aConfig = aServer->m_Config;
  cJSON* aRootJson = GRBL_ReceiveJSON(req);
  if( aRootJson == NULL ){
    ESP_LOGE(TAG,"GRBL_SetSettingsGrbl::Can't parse JSON file");
    return ESP_FAIL;
  }
  cJSON* aJson = cJSON_GetObjectItem(aRootJson,TYPE_SELECTOR);
  if( aJson ){
    char* aVal = cJSON_GetStringValue(aJson);
    bool isAP = false;
    if( strcmp(aVal,AP_VAL)==0 ){
      aConfig->m_isAccessPoint = true;
    }
    GRBL_SetIsWiFiAP(aConfig, isAP);
  }
  aRetVal = grbl_SetGroupData(aRootJson,aConfig,false);
  if( aRetVal != ESP_OK ){
    cJSON_Delete(aRootJson);
    return aRetVal;
  }
  aRetVal = grbl_SetGroupData(aRootJson,aConfig,true);
  if( aRetVal != ESP_OK ){
    cJSON_Delete(aRootJson);
    return aRetVal;
  }
  cJSON_Delete(aRootJson);
  return ESP_OK;
}