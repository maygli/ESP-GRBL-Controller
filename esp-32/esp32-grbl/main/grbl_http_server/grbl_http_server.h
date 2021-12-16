#ifndef GRBL_HTTP_SERVER
#define GRBL_HTTP_SERVER

#include <esp_http_server.h>
#include "../grbl_board_default_config.h"
#include "../grbl_config_manager.h"

typedef struct _GRBLServer{
    httpd_handle_t      m_HttpServer;
    GRBLBoardConfig*    m_Config;
    char                m_FileBuffer[HTTP_BUFFER_SIZE];
    char                m_Path[MAX_PATH_SIZE+1];
    int                 m_BasePathSize;
    bool                m_IsInitialised;
} GRBLServer;

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t GRBL_ServerStart(GRBLServer* theServer);
esp_err_t GRBL_ServerStop(GRBLServer* theServer);

#ifdef __cplusplus
}
#endif

#endif