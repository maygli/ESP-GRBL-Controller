#ifndef GRBL_HTTP_SD_H
#define GRBL_HTTP_SD_H

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t GRBL_HttpSendSDData(httpd_req_t *req);
esp_err_t GRBL_HttpRemoveFile(httpd_req_t *req);
esp_err_t GRBL_HttpCreateNewFolder(httpd_req_t *req);
esp_err_t GRBL_HttpRename(httpd_req_t *req);
esp_err_t GRBL_UploadSDFile(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif