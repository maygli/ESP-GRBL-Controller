#ifndef GRBL_HTTP_UTILS_H
#define GRBL_HTTP_UTILS_H

#include <esp_err.h>
#include <esp_http_server.h>

#include <cJSON.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t GRBL_SetReqTypeByExt(httpd_req_t *req, const char *filename);
int GRBL_AppendNPath(char* theBuffer, int theBaseSize, char* theFileName, int theFileNameSize);
int GRBL_AppendPath(char* theBuffer, int theBaseSize, char* theFileName);
cJSON* GRBL_ReceiveJSON(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif