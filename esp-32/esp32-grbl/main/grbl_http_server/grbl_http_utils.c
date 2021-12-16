#include <ctype.h>
#include <string.h>
#include <esp_log.h>
#include <sys/param.h>

#include <esp_http_server.h>

#include "grbl_http_utils.h"
#include "grbl_http_server.h"
#include "../grbl_board_default_config.h"

#define BOUNDARY_NAME "boundary"
#define BOUNDARY_SIZE 8

#define IS_FILE_EXT(filename, ext) \
    (strcasecmp(&filename[strlen(filename) - sizeof(ext) + 1], ext) == 0)

/* Set HTTP response content type according to file extension */
esp_err_t GRBL_SetReqTypeByExt(httpd_req_t *req, const char *filename)
{
    if (IS_FILE_EXT(filename, ".pdf")) {
        return httpd_resp_set_type(req, "application/pdf");
    } else if (IS_FILE_EXT(filename, ".html")) {
        return httpd_resp_set_type(req, "text/html");
    } else if (IS_FILE_EXT(filename, ".jpeg")) {
        return httpd_resp_set_type(req, "image/jpeg");
    } else if (IS_FILE_EXT(filename, ".ico")) {
        return httpd_resp_set_type(req, "image/x-icon");
    } else if (IS_FILE_EXT(filename, ".css")) {
        return httpd_resp_set_type(req, "text/css");
    } else if (IS_FILE_EXT(filename, ".js")) {
        return httpd_resp_set_type(req, "application/javascript");
    }
    /* This is a limited set only */
    /* For any other type always set as plain text */
    return httpd_resp_set_type(req, "text/plain");
}

int GRBL_AppendNPath(char* theBuffer, int theBaseSize, char* theFileName, int theFileNameSize)
{
    int aSize = theBaseSize;
    if( theBaseSize >= MAX_PATH_SIZE ){
        return -1;
    }
    if( theFileNameSize == 0 ){
        return theBaseSize;
    }
    if( ( theBuffer[aSize-1] != '/' ) && ( theFileName[0] != '/' )){
        theBuffer[aSize] = '/';
        aSize++;
    }
    if( (aSize + theFileNameSize) > MAX_PATH_SIZE )
        return -1;
    if( theFileNameSize > 0 ){
        strncpy(theBuffer+aSize,theFileName, theFileNameSize);
        aSize += theFileNameSize;
    }
    theBuffer[aSize] = 0;
    return aSize;
}

int GRBL_AppendPath(char* theBuffer, int theBaseSize, char* theFileName)
{
    int aFileNameSize = 0;
    if( theFileName != NULL ){
        aFileNameSize = strlen(theFileName);
    }
    return GRBL_AppendNPath(theBuffer, theBaseSize, theFileName, aFileNameSize);
}

cJSON* GRBL_ReceiveJSON(httpd_req_t *req)
{
    int             aReceived;
    cJSON           *aJson = NULL;
    GRBLServer*     aServer = (GRBLServer*)req->user_ctx;
    while(1){
        if ((aReceived = httpd_req_recv(req, aServer->m_FileBuffer, HTTP_BUFFER_SIZE)) <= 0) {
            if (aReceived == HTTPD_SOCK_ERR_TIMEOUT) {
                    /* Retry if timeout occurred */
                continue;
            }

            ESP_LOGE(TAG, "Can't receive data");
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Can't receive data");
            return NULL;
        }
        break;
    }
    aServer->m_FileBuffer[aReceived] = 0;
    ESP_LOGI(TAG,"Received=%s", aServer->m_FileBuffer);
    aJson = cJSON_Parse(aServer->m_FileBuffer);
    if( !aJson ){
        ESP_LOGE(TAG, "Can't parse JSON");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Can't parse JSON");
    }
    return aJson;
}