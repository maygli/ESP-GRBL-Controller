#include <esp_err.h>
#include <esp_log.h>
#include <esp_http_server.h>

#include <cJSON.h>

#include "../grbl_config_manager.h"
#include "grbl_http_server.h"
#include "grbl_http_utils.h"

#define VAL_BUFFER_SIZE 16
#define NAME_BUFFER_SIZE 32

esp_err_t GRBL_SendSettingsGrbl(httpd_req_t *req)
{
    char aNameBuffer[NAME_BUFFER_SIZE];
    char aValBuffer[VAL_BUFFER_SIZE];
    esp_err_t aRetCode;
    GRBLServer* aServer = (GRBLServer*)req->user_ctx;
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send_chunk(req,"{",1);
    bool isFirst = true;
    for( int i = 0 ; i < GRBL_PARS_MAX_INDEX ; i++ ){
      aRetCode = GRBL_GetGRBLParameterValue(aServer->m_Config,i,aValBuffer);
      if( aRetCode == ESP_OK ){
        if( !isFirst ){
          httpd_resp_send_chunk(req,",",1);
        }
        isFirst = false;
        sprintf(aNameBuffer,"\"grbl%d\":", i);
        httpd_resp_send_chunk(req,aNameBuffer,strlen(aNameBuffer));
        httpd_resp_send_chunk(req,"\"",1);
        httpd_resp_send_chunk(req,aValBuffer,strlen(aValBuffer));
        httpd_resp_send_chunk(req,"\"",1);
      }
    }
    httpd_resp_send_chunk(req,"}",1);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t GRBL_SetSettingsGrbl(httpd_req_t *req)
{
  esp_err_t aRetVal;
  GRBLServer* aServer = (GRBLServer*)req->user_ctx;
  cJSON* aRootJson = GRBL_ReceiveJSON(req);
  if( aRootJson == NULL ){
    ESP_LOGE(TAG,"GRBL_SetSettingsGrbl::Can't parse JSON file");
    return ESP_FAIL;
  }
  ESP_LOGI(TAG,"GRBL_SetSettingsGrbl::GRBL received");
  cJSON* aJson = aRootJson->child;
  while( aJson != NULL ){
    char* aValue = cJSON_GetStringValue(aJson);
    int anIndx;
    sscanf(aJson->string, "grbl%d", &anIndx);
    ESP_LOGI(TAG, "Index=%d, Value=%s", anIndx, aValue);
    aRetVal = GRBL_SetGRBLParameterValue(aServer->m_Config, anIndx, aValue);
    if( aRetVal != ESP_OK ){
      ESP_LOGE(TAG,"Error set parameter %d, value=%s", anIndx, aValue);
      cJSON_Delete(aRootJson);
      return ESP_FAIL;
    }
    aJson = aJson->next;
  }
  cJSON_Delete(aRootJson);
  httpd_resp_sendstr(req, "GRBL parameters set successfuly");
  return ESP_OK;
}
