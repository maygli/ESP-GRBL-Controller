#ifndef GRBL_HTTP_GET_FILE_H
#define GRBL_HTTP_GET_FILE_H

#include <esp_err.h>
#include <esp_http_server.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t GRBL_SendFile(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif