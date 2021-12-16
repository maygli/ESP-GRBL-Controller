/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <esp_event.h>

#include "grbl_board_default_config.h"
#include "grbl_sd_card_manager.h"
#include "grbl_config_manager.h"
#include "grbl_wifi.h"
#include "grbl_file_system.h"
#include "grbl_updater.h"
#include "grbl_http_server.h"

void initInternalFlash(GRBLBoardConfig* theConfig)
{
    static wl_handle_t aWLHandle = WL_INVALID_HANDLE;
    ESP_LOGI(TAG, "Mounting internal flash FAT filesystem");
    // To mount device we need name of device partition, define base_path
    // and allow format partition in case if it is new one and was not formated before
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount(GRBL_GetInternalFlashPrefix(theConfig), INTERNAL_FLASH_PARTITION, &mount_config, &aWLHandle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "Mount complete");
}

void printTest()
{
    DIR* aDir = opendir(INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME);
    if( !aDir ){
        ESP_LOGE(TAG,"printTest: Can't open dir %s", INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME);
        return ESP_FAIL;
    }
    while(true){
        struct dirent* de = readdir(aDir);
        if( !de ){
            break;
        }
        ESP_LOGI(TAG," Found entity %s type=%d", de->d_name, de->d_type);
    }
    closedir(aDir);    
}

void app_main(void)
{
    printf("Hello world!\n");
    SDManager aSDMgr;
    GRBLBoardConfig aConfig;
    GRBLServer aServer;
    aServer.m_Config = &aConfig;
    aSDMgr.m_Config = &aConfig;
    initInternalFlash(&aConfig);
    GRBL_InitConfigManager(&aConfig);
    GRBL_NewSDManager(&aSDMgr);
    GRBL_InitSDCardManager(&aSDMgr);
    GRBL_RunSDManager(&aSDMgr);
    GRBL_ConfigUpdateByINI(&aConfig, SD_BASE_PATH"/config.ini");
    GRBL_ConfigSaveINI(&aConfig,SD_BASE_PATH"/config_upd.ini");
    GRBL_UpdateControllerHTML();
    printTest();
/*    esp_err_t anErr = GRBL_FCopy(SD_BASE_PATH"/test_folder",SD_BASE_PATH"/internal_test1_folder");
    ESP_LOGI(TAG,"GRBL_FCopy returns %d", anErr);
    anErr = GRBL_RmTree(SD_BASE_PATH"/internal_test1_folder");
    ESP_LOGI(TAG,"Rmtree returns %d", anErr);
    if( anErr == ESP_OK ){
        FILE* aDstFile = fopen(INTERNAL_FLASH_BASE_PATH"/internal_test.txt","r");
        if( aDstFile ){
            char aBuff[1024];
            int aRd = fread(aBuff,1,1024,aDstFile);
            aBuff[aRd] = 0;
            ESP_LOGI(TAG, "Read file=%s", aBuff);
            fclose(aDstFile);
        }
    }*/
     /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    GRBL_WiFiStart(&aServer);
    while(1){
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
//    esp_restart();
}
