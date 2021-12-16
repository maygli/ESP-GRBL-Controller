
#include <esp_err.h>
#include <esp_log.h>
#include <esp_http_server.h>

#include "../grbl_board_default_config.h"
#include "grbl_default_page.h"
#include "grbl_http_server.h"
#include "grbl_http_get_file.h"
#include "grbl_http_upload_file.h"
#include "grbl_http_board_info.h"
#include "grbl_http_settings_wifi.h"
#include "grbl_http_settings_grbl.h"
#include "grbl_http_sd.h"

esp_err_t GRBL_ServerStart(GRBLServer* theServer)
{
    if( theServer == NULL ){
        ESP_LOGE(TAG,"GRBL_ServerStart: Can't init server. Pointer to structure is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    strcpy(theServer->m_Path,INTERNAL_FLASH_BASE_PATH"/"HTML_DIR_NAME"/");
    theServer->m_BasePathSize = strlen(theServer->m_Path);
    ESP_LOGI(TAG,"Server base path=%s", theServer->m_Path);
    theServer->m_HttpServer = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Use the URI wildcard matching function in order to
     * allow the same handler to respond to multiple different
     * target URIs which match the wildcard scheme */
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 16;
    
    ESP_LOGI(TAG, "Starting HTTP Server");
    if (httpd_start(&theServer->m_HttpServer, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start file server!");
        return ESP_FAIL;
    }

    httpd_uri_t aSendBoardInfo = {
        .uri       = "/board_info",  // board info
        .method    = HTTP_GET,
        .handler   = GRBL_SendBoardInfo,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aSendBoardInfo);

    httpd_uri_t aSendSettingsWifi = {
        .uri       = "/settings_wifi",  // wifi settings
        .method    = HTTP_GET,
        .handler   = GRBL_SendSettingsWiFi,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aSendSettingsWifi);

    httpd_uri_t aSendSettingsGrbl = {
        .uri       = "/settings_grbl",  // grbl settings
        .method    = HTTP_GET,
        .handler   = GRBL_SendSettingsGrbl,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aSendSettingsGrbl);

    httpd_uri_t aSendSDData = {
        .uri       = "/sd_filesystem_data",  // file system data
        .method    = HTTP_GET,
        .handler   = GRBL_HttpSendSDData,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aSendSDData);

    /* get html */
    httpd_uri_t aGetHtml = {
        .uri       = "*",  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = GRBL_SendFile,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aGetHtml);

    httpd_uri_t aRemoveFile = {
        .uri       = "/remove_file",  // Remove file on SD
        .method    = HTTP_POST,
        .handler   = GRBL_HttpRemoveFile,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aRemoveFile);

    httpd_uri_t aCreateNewFolder = {
        .uri       = "/create_new_folder",  // Create new folder on SD
        .method    = HTTP_POST,
        .handler   = GRBL_HttpCreateNewFolder,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aCreateNewFolder);

    httpd_uri_t aRenameFile = {
        .uri       = "/rename_file",  // Rename file on SD
        .method    = HTTP_POST,
        .handler   = GRBL_HttpRename,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aRenameFile);

    httpd_uri_t aSDUploadFile = {
        .uri       = "/sd_upload",  // Rename file on SD
        .method    = HTTP_POST,
        .handler   = GRBL_UploadSDFile,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aSDUploadFile);

    httpd_uri_t aGetSettingsGrbl = {
        .uri       = "/settings_grbl",  // grbl settings
        .method    = HTTP_POST,
        .handler   = GRBL_SetSettingsGrbl,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aGetSettingsGrbl);

    httpd_uri_t aSetSettingsWifi = {
        .uri       = "/settings_wifi",  // wifi settings
        .method    = HTTP_POST,
        .handler   = GRBL_SetSettingsWiFi,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aSetSettingsWifi);

    httpd_uri_t aFileUpload = {
        .uri       = "*",  // Match all URIs of type /path/to/file
        .method    = HTTP_POST,
        .handler   = GRBL_UploadFile,
        .user_ctx  = theServer    // Pass server data as context
    };
    httpd_register_uri_handler(theServer->m_HttpServer, &aFileUpload);

    return ESP_OK;
}

esp_err_t GRBL_ServerStop(GRBLServer* theServer)
{
    if( theServer == NULL ){
        ESP_LOGE(TAG,"GRBL_ServerStart: Can't init server. Pointer to structure is NULL");
        return ESP_ERR_INVALID_ARG;
    }
    if( theServer->m_HttpServer == NULL ){
        return ESP_OK;
    }
    httpd_stop(theServer->m_HttpServer);
    return ESP_OK;
}
