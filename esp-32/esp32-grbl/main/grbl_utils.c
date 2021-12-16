#include <esp_err.h>
#include <esp_log.h>
#include <string.h>

#include "grbl_board_default_config.h"

esp_err_t GRBL_ConvertDigit(char* theStr, unsigned int* theRes)
{
    *theRes = 0;
    int aCnt;
    if( strncmp(theStr,"0x", 2) == 0 ){
        aCnt = sscanf(theStr,"%x", theRes);
        if( aCnt != 1 ){
            ESP_LOGE(TAG,"Error. Can't convert string value '%s'", theStr);
            return ESP_FAIL;
        }
    }
    else if( strncmp(theStr,"0b",2) == 0){
        int anIndx = 2;
        while(theStr[anIndx] != 0){
            (*theRes) = (*theRes) << 1;
            if( theStr[anIndx] == '1'){
                (*theRes) = (*theRes) | 1;
            }
            anIndx++;                
        }
    }
    else{
        aCnt = sscanf(theStr,"%d", theRes);
        if( aCnt != 1 ){
            ESP_LOGE(TAG,"Error. Can't convert string value '%s'", theStr);
            return ESP_FAIL;
        }
    }
    ESP_LOGI(TAG, "Converted=%d", (*theRes));
    return ESP_OK;
}