#ifndef GRBL_SD_CARD_MANAGER
#define GRBL_SD_CARD_MANAGER

#include "esp_err.h"
#include "driver/sdspi_host.h"

#include "grbl_config_manager.h"

typedef struct _SDManager{
    sdmmc_card_t*       m_Card;
    int                 m_HostId;
    int                 m_PrevState;
    GRBLBoardConfig*    m_Config;
} SDManager;

#ifdef __cplusplus
extern "C" {
#endif

void GRBL_RunSDManager( SDManager* theMgr );
void GRBL_NewSDManager(SDManager* theMgr);
esp_err_t GRBL_InitSDCardManager(SDManager* theMgr);

#ifdef __cplusplus
}
#endif

#endif