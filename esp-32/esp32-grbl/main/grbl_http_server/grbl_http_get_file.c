#include <esp_log.h>
#include <esp_err.h>
#include <sys/stat.h>

#include "grbl_http_server.h"
#include "grbl_http_get_file.h"
#include "grbl_default_page.h"
#include "grbl_http_utils.h"
#include "../grbl_board_default_config.h"

static esp_err_t grbl_SendDefaultPage(httpd_req_t *req)
{
    httpd_resp_sendstr(req, GRBL_DEFAULT_PAGE);
    return ESP_OK;     
}

esp_err_t GRBL_SendFile(httpd_req_t *req)
{
    esp_err_t   aRes;
    int         aSize;
    struct stat aStat;
    GRBLServer* aServer = ((GRBLServer*)req->user_ctx);

    ESP_LOGI(TAG,"Get file: '%s'", req->uri);
    if( ( req->uri == NULL ) ||
        ( strcmp(req->uri,"/") == 0 ) ||
        ( strcmp(req->uri,"DEFAULT_PAGE") == 0)){
        aSize = GRBL_AppendNPath(aServer->m_Path, aServer->m_BasePathSize, DEFAULT_PAGE, strlen(DEFAULT_PAGE));
        if( aSize < 0 ){
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
            return ESP_FAIL;
        }
        aRes = stat(aServer->m_Path,&aStat);
        if( aRes != ESP_OK ){
            aRes = grbl_SendDefaultPage(req);
            return aRes;
        }
    }
    else{
        aSize = GRBL_AppendNPath(aServer->m_Path, aServer->m_BasePathSize, req->uri, strlen(req->uri));
        if( aSize < 0 ){
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
            return ESP_FAIL;
        }
    }
    ESP_LOGI(TAG,"Request file '%s'", aServer->m_Path);
    aRes = stat(aServer->m_Path,&aStat);
    if( aRes != ESP_OK ){
        ESP_LOGE(TAG,"Requested file '%s' not found", aServer->m_Path);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Not found");
        return aRes;
    }
    FILE* aFile = fopen(aServer->m_Path,"r");
    if( !aFile ){
        ESP_LOGE(TAG, "Failed to read existing file : %s", aServer->m_Path);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
    }

    GRBL_SetReqTypeByExt(req,aServer->m_Path);

    do {
        /* Read file in chunks into the scratch buffer */
        aSize = fread(aServer->m_FileBuffer, 1, HTTP_BUFFER_SIZE, aFile);

        if (aSize > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, aServer->m_FileBuffer, aSize) != ESP_OK) {
                fclose(aFile);
                ESP_LOGE(TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
           }
        }

        /* Keep looping till the whole file is sent */
    } while (aSize != 0);

    /* Close file after sending complete */
    fclose(aFile);
    ESP_LOGI(TAG, "File sending complete");

    /* Respond with an empty chunk to signal HTTP response completion */
#ifdef CONFIG_EXAMPLE_HTTPD_CONN_CLOSE_HEADER
    httpd_resp_set_hdr(req, "Connection", "close");
#endif
    httpd_resp_send_chunk(req, NULL, 0);    
    return ESP_OK;
}
