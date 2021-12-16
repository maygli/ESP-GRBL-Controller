
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "dirent.h"
#include <cJSON.h>

#include "grbl_sd_card_manager.h"
#include "grbl_board_default_config.h"
#include "grbl_config_manager.h"

esp_err_t grbl_DeinitSDCardManager(SDManager* theMgr);
int grbl_IsSDCardManagerInit(SDManager* theMgr);
void grbl_SdCardTask(void* theParam);

void grbl_SdCardTask(void* theParam)
{
    SDManager* aMgr = (SDManager*)theParam;
    esp_err_t aRes;

    while(1){
        int aLevel = gpio_get_level(GRBL_GetSDDetPin(aMgr->m_Config));
        if( aLevel != aMgr->m_PrevState ){
            aMgr->m_PrevState = aLevel;
            if( aMgr->m_PrevState ){
                if( grbl_IsSDCardManagerInit(aMgr) ){
                    aRes = grbl_DeinitSDCardManager(aMgr);
                    if( aRes != ESP_OK ){
                        ESP_LOGE(TAG,"Can't init SD Card\n");
                        grbl_DeinitSDCardManager(aMgr);
                    }
                    vTaskDelay(SD_CARD_MANAGER_GUARD_TIME_MS/portTICK_PERIOD_MS);
                }
            }
            else{
                if( !grbl_IsSDCardManagerInit(aMgr) ){
                    aRes = GRBL_InitSDCardManager(aMgr);
                    if( aRes != ESP_OK ){
                        ESP_LOGE(TAG,"Can't deinit SD Card\n");
                    }
                }
                vTaskDelay(SD_CARD_MANAGER_GUARD_TIME_MS/portTICK_PERIOD_MS);
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
     }
     vTaskDelete( NULL );
}

esp_err_t grbl_DeinitSDCardManager(SDManager* theMgr)
{
    if( !grbl_IsSDCardManagerInit(theMgr) )
        return ESP_OK;
    esp_vfs_fat_sdcard_unmount(GRBL_GetSDPrefix(theMgr->m_Config), theMgr->m_Card);
    ESP_LOGI(TAG, "Card unmounted");
    spi_bus_free(theMgr->m_HostId);
    theMgr->m_Card = NULL;    
    return ESP_OK;
}

int grbl_IsSDCardManagerInit(SDManager* theMgr)
{
    if( theMgr->m_Card ){
        return 1;
    }
    return 0;
}

void GRBL_NewSDManager(SDManager* theMgr)
{
    theMgr->m_PrevState=1;
    theMgr->m_Card = NULL;
    theMgr->m_HostId = 0;
}

esp_err_t GRBL_InitSDCardManager(SDManager* theMgr)
{
    ESP_LOGI(TAG, "Initializing SD card");
    esp_err_t ret;
    if( grbl_IsSDCardManagerInit(theMgr) )
        return ESP_OK;
    GRBL_NewSDManager(theMgr);
    theMgr->m_PrevState = gpio_get_level(GRBL_GetSDDetPin(theMgr->m_Config));
    if( theMgr->m_PrevState ){
        return ESP_ERR_INVALID_STATE;
    }
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    // Use settings defined above to initialize SD card and mount FAT filesystem.
    // Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience functions.
    // Please check its source code and implement error recovery when developing
    // production applications.
    ESP_LOGI(TAG, "Using SPI peripheral");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = GRBL_GetSDMOSIPin(theMgr->m_Config),
        .miso_io_num = GRBL_GetSDMISOPin(theMgr->m_Config),
        .sclk_io_num = GRBL_GetSDSCKPin(theMgr->m_Config),
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return ret;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = GRBL_GetSDCSPin(theMgr->m_Config);
    slot_config.gpio_cd = GRBL_GetSDDetPin(theMgr->m_Config);
    slot_config.host_id = host.slot;

    theMgr->m_HostId = slot_config.host_id;

    ret = esp_vfs_fat_sdspi_mount(GRBL_GetSDPrefix(theMgr->m_Config), &host, &slot_config, &mount_config, &theMgr->m_Card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return ret;
    }

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, theMgr->m_Card);

    int aNeedReset=0;
    FILE* aCfg = fopen(SD_BASE_PATH"/"CONFIG_FILE_NAME,"r");
    if( aCfg ){
        ESP_LOGI(TAG, "Config file found on SD card. Need reset board");
        fclose(aCfg);
        aNeedReset = 1;
    }
    if( !aNeedReset ){
        FILE* aFirmware = fopen(SD_BASE_PATH"/"FIRMWARE_FILE_NAME,"r");
        if( aFirmware ){
            ESP_LOGI(TAG, "Firmware file found on SD card. Need reset board");
            fclose(aFirmware);
            aNeedReset = 1;
        }
    }
    if( !aNeedReset ){
        DIR *aHTMLDir = opendir(SD_BASE_PATH"/"HTML_DIR_NAME);
        if( aHTMLDir ){
            ESP_LOGI(TAG, "HTML dir found. Need reset board");
            closedir(aHTMLDir);
            aNeedReset = 1;
        }
    }


    return ESP_OK;
}

void GRBL_RunSDManager( SDManager* theMgr )
{
    xTaskCreatePinnedToCore(grbl_SdCardTask, "SDManager", 4096, (void*)theMgr, 1, NULL, SD_MANAGER_CORE);
}