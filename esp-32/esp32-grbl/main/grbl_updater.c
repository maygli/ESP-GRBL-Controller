#include <esp_err.h>
#include <sys/stat.h>
#include <esp_log.h>

#include "grbl_updater.h"
#include "grbl_board_default_config.h"
#include "grbl_file_system.h"

esp_err_t GRBL_UpdateControllerHTMLPLR()
{
    esp_err_t aRes;
    struct stat aStat;
    aRes = stat(INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME"."PROCESSED_EXTENSION,&aStat);
    if( aRes != ESP_OK ){
        return ESP_OK;
    }
    GRBL_RmTree(INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME);
    aRes = rename(INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME"."PROCESSED_EXTENSION, INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME);
    if( aRes != ESP_OK ){
        ESP_LOGI(TAG,"GRBL_UpdateControllerHTMLPLR::Can't restore html folder");
        return aRes;
    }
    return aRes;
}

esp_err_t GRBL_UpdateControllerHTML()
{
    esp_err_t aRes; 
    struct stat aStat;
    aRes = stat(SD_BASE_PATH"/"HTML_DIR_NAME,&aStat);
    ESP_LOGI(TAG,"GRBL_UpdateControllerHTML");
    if( aRes != ESP_OK ){
        ESP_LOGI(TAG,"GRBL_UpdateControllerHTML::controller_html not found");
        return GRBL_UpdateControllerHTMLPLR();
    }
    if(!S_ISDIR(aStat.st_mode)){
        ESP_LOGI(TAG,"GRBL_UpdateControllerHTML::controller_html is not directory");
        return GRBL_UpdateControllerHTMLPLR();
    }
    ESP_LOGI(TAG,"GRBL_UpdateControllerHTML::html folder found");
    aRes = stat(INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME"."PROCESSED_EXTENSION,&aStat);
    if( aRes != ESP_OK ){
        aRes = stat(INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME,&aStat);
        if( aRes == ESP_OK ){
            aRes = rename(INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME, INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME"."PROCESSED_EXTENSION);
            if( aRes != ESP_OK ){
                ESP_LOGE(TAG,"GRBL_UpdateControllerHTML::can't rename file on iternal flash");
                return aRes;
            }
        }
    }
    aRes = GRBL_FCopy(SD_BASE_PATH"/"HTML_DIR_NAME,INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG,"GRBL_UpdateControllerHTML::can't copy html folder from SD card to internal flash");
        return aRes;
    }
    aRes = GRBL_RmTree(INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME"."PROCESSED_EXTENSION);
    aRes = GRBL_RmTree(SD_BASE_PATH"/"HTML_DIR_NAME"."PROCESSED_EXTENSION);
    aRes = rename(SD_BASE_PATH"/"HTML_DIR_NAME,SD_BASE_PATH"/"HTML_DIR_NAME"."PROCESSED_EXTENSION);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG, "GRBL_UpdateControllerHTML::can't rename html folder on SD card");
        return aRes;
    }
    return aRes;
}